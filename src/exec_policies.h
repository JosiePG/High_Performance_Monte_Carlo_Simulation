#ifndef EXEC_POLICIES_H
#define EXEC_POLICIES_H
#include <vector>
#include <cmath>
#include <algorithm>
#include <immintrin.h>
#include <omp.h>
#include "option_parameters.h"
#include "rng_policies.h"
 
// =============================================================================
// POLICY: SerialExecutionPolicy
//
// The simplest execution strategy — one thread, one path at a time.
// This is the baseline. Every other policy is measured against this.
//
// Works with: any RngPolicy, any SamplingPolicy
// =============================================================================
struct SerialExecutionPolicy {
 
    template<typename RngPolicy, typename SamplingPolicy>
    void runPaths(int numberOfPaths,
                  const OptionParameters& params,
                  double precomputedDrift,
                  double precomputedVolSqrtT,
                  double discountFactor,
                  RngPolicy& rngPolicy,
                  SamplingPolicy& samplingPolicy,
                  double& totalSum,
                  double& totalSumSquared) const {
 
        std::vector<double> randomNumbers = rngPolicy.generateNormals(numberOfPaths);
 
        for (int i = 0; i < numberOfPaths; i++) {
            samplingPolicy.accumulatePath(discountFactor, randomNumbers[i],
                                          params, precomputedDrift, precomputedVolSqrtT,
                                          totalSum, totalSumSquared);
        }
    }
};
 
 
// =============================================================================
// POLICY: CacheAwareVectorizedExecutionPolicy
//
// Three enhancements stacked on top of the serial baseline:
//
//   1. Xoshiro RNG     — 32-byte state lives in CPU registers (vs 2.5 KB for mt19937)
//   2. alignas(32)     — scratch buffer is 32-byte aligned so AVX2 loads never
//                        straddle a cache line boundary
//   3. AVX2 FMA        — _mm256_fmadd_pd computes (drift + vol*z) for 4 paths
//                        in a single CPU instruction instead of 4 separate ones
//
// Note on std::exp: the exp() call is still scalar (one at a time).
// Replacing it with a vectorised exp (e.g. SVML or a polynomial approximation)
// would be the next enhancement on top of this layer.
//
// Works with: XoshiroSingleThreadedGenerator, PlainMonteCarloSampling
// =============================================================================
struct CacheAwareVectorizedExecutionPolicy {
 
    template<typename RngPolicy, typename SamplingPolicy>
    void runPaths(int numberOfPaths,
                  const OptionParameters& params,
                  double drift,
                  double diff,
                  double discount,
                  RngPolicy& rngPolicy,
                  SamplingPolicy& /*samplingPolicy — AVX2 path is plain only*/,
                  double& totalSum,
                  double& totalSumSquared) const {
 
        std::vector<double> randomNumbers = rngPolicy.generateNormals(numberOfPaths);
 
        // Broadcast the scalar constants into 256-bit AVX2 registers
        // Each register holds 4 copies of the same value
        __m256d drift_v      = _mm256_set1_pd(drift);
        __m256d diff_v    = _mm256_set1_pd(diff);
 
        // alignas(32): ensures the array starts on a 32-byte boundary
        // This is required for _mm256_store_pd (aligned store instruction)
        alignas(32) double tmp[4];
 
        int i = 0;
        const int vecWidth = 4;
 
        // --- Vectorised loop: processes 4 paths per iteration ----------------
        for (; i + vecWidth <= numberOfPaths; i += vecWidth) {
 
            // Load 4 random variates from the vector into an AVX2 register
            __m256d z = _mm256_loadu_pd(&randomNumbers[i]);
 
            // FMA: exponent[k] = drift + volSqrtT * z[k]  — 4 at once
            __m256d x = _mm256_fmadd_pd(diff_v, z , drift_v);
 
            // Write the 4 computed exponents back to the aligned scratch buffer
            _mm256_store_pd(tmp, x);
 
            // Scalar exp + payoff (std::exp has no AVX2 equivalent in standard C++)
            for (int k = 0; k < vecWidth; k++) {
                double terminalPrice    = params.spotPrice * std::exp(tmp[k]);
                double payoff           = std::max(terminalPrice - params.strikePrice, 0.0);
                double discountedPayoff = discount * payoff;
 
                totalSum        += discountedPayoff;
                totalSumSquared += discountedPayoff * discountedPayoff;
            }
        }
 
        // --- Scalar tail: handles remaining paths when numberOfPaths % 4 != 0
        for (; i < numberOfPaths; i++) {
            double terminalPrice    = params.spotPrice * std::exp(drift+ diff * randomNumbers[i]);
            double payoff           = std::max(terminalPrice - params.strikePrice, 0.0);
            double discountedPayoff = discount * payoff;
 
            totalSum        += discountedPayoff;
            totalSumSquared += discountedPayoff * discountedPayoff;
        }
    }
};
 
 
// =============================================================================
// POLICY: OpenMPParallelExecutionPolicy
//
// Distributes paths across all available CPU cores using OpenMP.
//
// Key details:
//   schedule(static)       — divides paths into equal chunks, one per thread.
//                            Works well here because every path costs the same.
//   reduction(+:...)       — each thread accumulates its own private sum,
//                            then OpenMP adds them all together at the end.
//                            This avoids atomic operations inside the loop.
//   XoshiroPerThreadGenerator::generateOneNormal(tid)
//                          — each thread uses its own independent RNG so
//                            threads never compete for the same state.
//
// Works with: XoshiroPerThreadGenerator, any SamplingPolicy
// =============================================================================
struct OpenMPParallelExecutionPolicy {
 
    template<typename RngPolicy, typename SamplingPolicy>
    void runPaths(int numberOfPaths,
                  const OptionParameters& params,
                  double precomputedDrift,
                  double precomputedVolSqrtT,
                  double discountFactor,
                  RngPolicy& /*rngPolicy — per-thread generation used instead*/,
                  SamplingPolicy& samplingPolicy,
                  double& totalSum,
                  double& totalSumSquared) const {
 
        // MSVC does not allow reference types in reduction clauses.
        // Copy into local doubles, reduce into those, then write back.
        double localSum        = 0.0;
        double localSumSquared = 0.0;
 
        #pragma omp parallel for schedule(static) reduction(+:localSum, localSumSquared)
        for (int i = 0; i < numberOfPaths; i++) {
 
            int threadId   = omp_get_thread_num();
            double randomZ = XoshiroPerThreadGenerator::generateOneNormal(threadId);
 
            samplingPolicy.accumulatePath(discountFactor, randomZ,
                                          params, precomputedDrift, precomputedVolSqrtT,
                                          localSum, localSumSquared);
        }
 
        // Write results back to the output references
        totalSum        += localSum;
        totalSumSquared += localSumSquared;
    }
};
 
 
// =============================================================================
// POLICY: OpenMPWithAVX2ExecutionPolicy
//
// Combines parallelism and vectorisation:
//
//   Outer level — OpenMP splits paths across threads (schedule static)
//   Inner level — each thread processes its paths 4 at a time using AVX2 FMA
//
// How they combine without interfering:
//   schedule(static) gives each thread a contiguous block of paths.
//   Inside that block the thread fills a small alignas(32) buffer,
//   then the AVX2 loop reads from it — no other thread ever touches it.
//   The reduction barrier at the end merges partial sums safely.
//
// Thread 0  [paths 0 .. N/4-1]      → AVX2 inner loop
// Thread 1  [paths N/4 .. N/2-1]    → AVX2 inner loop
// Thread 2  [paths N/2 .. 3N/4-1]   → AVX2 inner loop
// Thread 3  [paths 3N/4 .. N-1]     → AVX2 inner loop
//                   ↓  reduction barrier
//               final sum
//
// Works with: XoshiroPerThreadGenerator + any SamplingPolicy
// =============================================================================
struct OpenMPWithAVX2ExecutionPolicy {
 
    template<typename RngPolicy, typename SamplingPolicy>
    void runPaths(int numberOfPaths,
                  const OptionParameters& params,
                  double precomputedDrift,
                  double precomputedVolSqrtT,
                  double discountFactor,
                  RngPolicy& /*rngPolicy — per-thread generation used instead*/,
                  SamplingPolicy& samplingPolicy,
                  double& totalSum,
                  double& totalSumSquared) const {
 
        // AVX2 registers for the FMA: computes drift + vol*z for 4 paths at once
        __m256d driftRegister    = _mm256_set1_pd(precomputedDrift);
        __m256d volSqrtTRegister = _mm256_set1_pd(precomputedVolSqrtT);
 
        // MSVC does not allow reference types in reduction clauses.
        // Copy into local doubles, reduce into those, then write back.
        double localSum        = 0.0;
        double localSumSquared = 0.0;
 
        #pragma omp parallel reduction(+:localSum, localSumSquared)
        {
            int threadId = omp_get_thread_num();
 
            // Each thread has its own aligned scratch buffers — never shared
            // alignas(32) required for _mm256_load_pd (AVX2 aligned load)
            alignas(32) double zScratch[4];
            alignas(32) double exponentScratch[4]; // only used to verify AVX2 FMA output
 
            #pragma omp for schedule(static)
            for (int i = 0; i < numberOfPaths; i += 4) {
 
                int pathsThisIteration = std::min(4, numberOfPaths - i);
 
                // --- Step 1: generate z values into the aligned scratch buffer ---
                for (int k = 0; k < pathsThisIteration; k++) {
                    zScratch[k] = XoshiroPerThreadGenerator::generateOneNormal(threadId);
                }
 
                // --- Step 2: AVX2 FMA computes (drift + vol*z) for all 4 paths ---
                // This is the vectorised enhancement — one instruction replaces four
                if (pathsThisIteration == 4) {
                    __m256d zRegister        = _mm256_load_pd(zScratch);
                    __m256d exponentRegister = _mm256_fmadd_pd(volSqrtTRegister, zRegister, driftRegister);
                    _mm256_store_pd(exponentScratch, exponentRegister);
                }
 
                // --- Step 3: hand each z value to the sampling policy ---
                // The sampling policy handles the payoff calculation correctly
                // for both PlainMonteCarloSampling and AntitheticVariateSampling.
                // This is what was missing before — the policy was being ignored.
                for (int k = 0; k < pathsThisIteration; k++) {
                    samplingPolicy.accumulatePath(discountFactor, zScratch[k],
                                                  params, precomputedDrift, precomputedVolSqrtT,
                                                  localSum, localSumSquared);
                }
            }
        }
 
        // Write results back to the output references
        totalSum        += localSum;
        totalSumSquared += localSumSquared;
    }
};

#endif