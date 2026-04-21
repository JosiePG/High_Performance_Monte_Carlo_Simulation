#ifndef EXEC_POLICIES_H
#define EXEC_POLICIES_H
#include <vector>
#include <cmath>
#include <algorithm>
#include <immintrin.h> // AVX intrinsics for SIMD vectorization
#include <omp.h> // OpenMP for parallel execution
#include "option_parameters.h"
#include "rng_policies.h"
 
// Baseline implementation
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
 
        // Generates all required random normal variables
        std::vector<double> randomNumbers = rngPolicy.generateNormals(numberOfPaths);
 
        // Loops over each Monte Carlo path
        for (int i = 0; i < numberOfPaths; i++) {
            // Hands off to whichever sampling policy was chosen at compile time
            samplingPolicy.accumulatePath(discountFactor, randomNumbers[i],
                                          params, precomputedDrift, precomputedVolSqrtT,
                                          totalSum, totalSumSquared);
        }
    }
};

 // Uses AVX (256-bit registers) to process 4 paths at once , as well as cache-aware memory alignment 

struct CacheAwareVectorizedExecutionPolicy {

    template<typename RngPolicy, typename SamplingPolicy>
    void runPaths(int numberOfPaths,
                  const OptionParameters& params,
                  double drift,
                  double diff,
                  double discount,
                  RngPolicy& rngPolicy,
                  SamplingPolicy&,  // Not used here (manual payoff computation is used instead)
                  double& totalSum,
                  double& totalSumSquared) const {
                    
                    std::vector<double> randomNumbers = rngPolicy.generateNormals(numberOfPaths);
                    // Broadcasts constants into AVX registers (vector of 4 doubles)
                    __m256d spot_v  = _mm256_set1_pd(params.spotPrice); 
                    __m256d drift_v = _mm256_set1_pd(drift);
                    __m256d diff_v  = _mm256_set1_pd(diff);


                    int i = 0;
                    const int vecWidth = 4;

                    // Aligned storage for vector results
                    alignas(32) double tmp[4];

                    // Main SIMD loop (processes 4 paths per iteration)
                    for (; i + vecWidth <= numberOfPaths; i += vecWidth)
                    {
                        // Loads 4 random normals into AVX register
                        __m256d z = _mm256_loadu_pd(&randomNumbers[i]);
                        // Computes drift + diff * z using fused multiply-add (FMA)
                        __m256d x = _mm256_fmadd_pd(diff_v, z, drift_v);
                        // Store results back to scalar array
                        _mm256_store_pd(tmp, x);

                        for (int k = 0; k < 4; ++k)
                        {
                            double ST = params.spotPrice * std::exp(tmp[k]);

                            // Computes payoff depending on option type
                            double payoff = (params.optionType == OptionType::CALL)
                            ? std::max(ST - params.strikePrice, 0.0)
                            : std::max(params.strikePrice - ST, 0.0);

                            double discounted_payoff = discount * payoff;
                            // Accumulates results
                            totalSum  += discounted_payoff;
                            totalSumSquared  += discounted_payoff * discounted_payoff;
                        }
                    }

                    // Tail loop to handle remaining paths (when numberOfPaths is not a multiple of 4)
                    for (; i < numberOfPaths; ++i)
                    {
                        double ST = params.spotPrice * std::exp(drift + diff * randomNumbers[i]);
                        double payoff = (params.optionType == OptionType::CALL)
                        ? std::max(ST - params.strikePrice, 0.0)
                        : std::max(params.strikePrice - ST, 0.0);
                        double discounted_payoff = discount * payoff;
                        totalSum  += discounted_payoff;
                        totalSumSquared  += discounted_payoff * discounted_payoff;
                    }


                  }
};

// Uses multi-threading (CPU cores) to parallelize paths
struct OpenMPParallelExecutionPolicy {
 
    template<typename RngPolicy, typename SamplingPolicy>
    void runPaths(int numberOfPaths,
                  const OptionParameters& params,
                  double drift,
                  double diff,
                  double discount,
                  RngPolicy& ,
                  SamplingPolicy& samplingPolicy,
                  double& totalSum,
                  double& totalSumSquared) const {
 
        // Local accumulators (required because OpenMP reductions
        // cannot directly use reference parameters in MSVC)
        double localSum        = 0.0;
        double localSumSquared = 0.0;
 
        // Parallel loop :
        // - schedule(static) evenly divides work across threads
        // - reduction safely accumulates results across threads
        #pragma omp parallel for schedule(static) reduction(+:localSum, localSumSquared)
        for (int i = 0; i < numberOfPaths; i++) {
 
            // Identifies thread (used for per-thread RNG)
            int threadId   = omp_get_thread_num();
            // Generates one normal random variable per path, per thread
            // This ensures thread-safe RNG usage
            double randomZ = XoshiroPerThreadGenerator::generateOneNormal(threadId);
 
            samplingPolicy.accumulatePath(discount, randomZ,
                                          params, drift, diff,
                                          localSum, localSumSquared);
        }
 
        // Combines thread-local results into global accumulators
        totalSum        += localSum;
        totalSumSquared += localSumSquared;
    }
};
 
 
#endif