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
                    __m256d spot_v  = _mm256_set1_pd(params.spotPrice); 
                    __m256d drift_v = _mm256_set1_pd(drift);
                    __m256d diff_v  = _mm256_set1_pd(diff);


                    int i = 0;
                    const int vecWidth = 4;

                    alignas(32) double tmp[4];

                    for (; i + vecWidth <= numberOfPaths; i += vecWidth)
                    {
                        __m256d z = _mm256_loadu_pd(&randomNumbers[i]);
                        __m256d x = _mm256_fmadd_pd(diff_v, z, drift_v);
                        _mm256_store_pd(tmp, x);

                        for (int k = 0; k < 4; ++k)
                        {
                            double ST = params.spotPrice * std::exp(tmp[k]);

                            double payoff = (params.optionType == OptionType::CALL)
                            ? std::max(terminalPrice - params.strikePrice, 0.0)
                            : std::max(params.strikePrice - terminalPrice, 0.0);
                            double discounted_payoff = discount * payoff;
                            totalSum  += discounted_payoff;
                            totalSumSquared  += discounted_payoff * discounted_payoff;
                        }
                    }

                    for (; i < numberOfPaths; ++i)
                    {
                        double ST = params.spotPrice * std::exp(drift + diff * randomNumbers[i]);
                        double payoff = (params.optionType == OptionType::CALL)
                      ? std::max(terminalPrice - params.strikePrice, 0.0)
                      : std::max(params.strikePrice - terminalPrice, 0.0);
                        double discounted_payoff = discount * payoff;
                        totalSum  += discounted_payoff;
                        totalSumSquared  += discounted_payoff * discounted_payoff;
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