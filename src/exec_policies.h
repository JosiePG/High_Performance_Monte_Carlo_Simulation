#ifndef EXEC_POLICIES_H
#define EXEC_POLICIES_H
#include <vector>
#include <cmath>
#include <algorithm>
#include <immintrin.h>
#include <omp.h>
#include <sleef.h>
#include "option_parameters.h"
#include "rng_policies.h"
 
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
 
 


struct CacheAwareVectorizedExecutionPolicy {

    template<typename RngPolicy, typename SamplingPolicy>
    void runPaths(int numberOfPaths,
                  const OptionParameters& params,
                  double drift,
                  double diff,
                  double discount,
                  RngPolicy& rngPolicy,
                  SamplingPolicy&,
                  double& totalSum,
                  double& totalSumSquared) const {

        std::vector<double> randomNumbers = rngPolicy.generateNormals(numberOfPaths);

        // Broadcast constants
        __m256d drift_v    = _mm256_set1_pd(drift);
        __m256d diff_v     = _mm256_set1_pd(diff);
        __m256d spot_v     = _mm256_set1_pd(params.spotPrice);
        __m256d strike_v   = _mm256_set1_pd(params.strikePrice);
        __m256d discount_v = _mm256_set1_pd(discount);
        __m256d zero_v     = _mm256_setzero_pd();

        // SIMD accumulators
        __m256d sum_v    = _mm256_setzero_pd();
        __m256d sumsq_v  = _mm256_setzero_pd();

        int i = 0;
        const int vecWidth = 4;

        // --- Fully vectorised loop ---
        for (; i + vecWidth <= numberOfPaths; i += vecWidth) {

            // Load randoms
            __m256d z = _mm256_loadu_pd(&randomNumbers[i]);

            // x = drift + diff * z
            __m256d x = _mm256_fmadd_pd(diff_v, z, drift_v);

            // Vectorized exp 
            __m256d exp_x = Sleef_expd4_u10(x);

            // terminal = S0 * exp(x)
            __m256d terminal = _mm256_mul_pd(spot_v, exp_x);

            // payoff = max(terminal - strike, 0)
            __m256d payoff = _mm256_max_pd(
                _mm256_sub_pd(terminal, strike_v),
                zero_v
            );

            // discounted payoff
            __m256d discounted = _mm256_mul_pd(discount_v, payoff);

            // accumulate
            sum_v   = _mm256_add_pd(sum_v, discounted);
            sumsq_v = _mm256_fmadd_pd(discounted, discounted, sumsq_v);
        }

        // --- Horizontal reduction (SIMD → scalar) ---
        alignas(32) double sum_arr[4];
        alignas(32) double sumsq_arr[4];

        _mm256_store_pd(sum_arr, sum_v);
        _mm256_store_pd(sumsq_arr, sumsq_v);

        for (int k = 0; k < 4; ++k) {
            totalSum        += sum_arr[k];
            totalSumSquared += sumsq_arr[k];
        }

        // --- Scalar tail ---
        for (; i < numberOfPaths; i++) {
            double x = drift + diff * randomNumbers[i];
            double terminalPrice    = params.spotPrice * std::exp(x);
            double payoff           = std::max(terminalPrice - params.strikePrice, 0.0);
            double discountedPayoff = discount * payoff;

            totalSum        += discountedPayoff;
            totalSumSquared += discountedPayoff * discountedPayoff;
        }
    }
};
struct OpenMPParallelExecutionPolicy {
 
    template<typename RngPolicy, typename SamplingPolicy>
    void runPaths(int numberOfPaths,
                  const OptionParameters& params,
                  double drift,
                  double diff,
                  double discount,
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
 
            samplingPolicy.accumulatePath(discount, randomZ,
                                          params, drift, diff,
                                          localSum, localSumSquared);
        }
 
        // Write results back to the output references
        totalSum        += localSum;
        totalSumSquared += localSumSquared;
    }
};
 
 
#endif