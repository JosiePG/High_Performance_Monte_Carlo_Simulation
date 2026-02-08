#include <algorithm>
#include "optimized_mc_engine.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <immintrin.h>
#include <vector>
#include "xoshiro.h"

std::vector<double> OptimizedMcEngine::generateRandomNormalVariables(int no_of_paths)  {

    std::vector<double> randomNumbers(no_of_paths);

    std::random_device rd; // seed
    xso::rng generator(rd()); // xoshiro random number generator
    std::normal_distribution<double> normal_dist(0.0,1);

    for (int i=0;i<no_of_paths;i++) {
        randomNumbers[i] = normal_dist(generator);
    }

    return randomNumbers;

}

std::pair<double,double> OptimizedMcEngine::runSimulation(int no_of_paths,double spotPrice,double strikePrice,double timeToMaturity,double riskFreeRate,double volatility)
{
    std::vector<double> randomNumbers =generateRandomNormalVariables(no_of_paths);
    const double drift =(riskFreeRate - 0.5 * volatility * volatility) * timeToMaturity;
    const double diff = volatility * std::sqrt(timeToMaturity);
    const double discount = std::exp(-riskFreeRate * timeToMaturity);

    __m256d spot_v  = _mm256_set1_pd(spotPrice); 
    __m256d drift_v = _mm256_set1_pd(drift);
    __m256d diff_v  = _mm256_set1_pd(diff);

    double sum = 0.0;
    double sum_squared  = 0.0;

    int i = 0;
    const int vecWidth = 4;

    alignas(32) double tmp[4];

    for (; i + vecWidth <= no_of_paths; i += vecWidth)
    {
        __m256d z = _mm256_loadu_pd(&randomNumbers[i]);
        __m256d x = _mm256_fmadd_pd(diff_v, z, drift_v);
        _mm256_store_pd(tmp, x);

        for (int k = 0; k < 4; ++k)
        {
            double ST = spotPrice * std::exp(tmp[k]);
            double payoff = std::max(ST - strikePrice, 0.0);
            double discounted_payoff = discount * payoff;
            sum  += discounted_payoff;
            sum_squared  += discounted_payoff * discounted_payoff;
        }
    }

    for (; i < no_of_paths; ++i)
    {
        double ST = spotPrice * std::exp(drift + diff * randomNumbers[i]);
        double payoff =std::max(ST - strikePrice, 0.0);
        double discounted_payoff = discount * payoff;
        sum  += discounted_payoff;
        sum_squared  += discounted_payoff * discounted_payoff;
    }

    double estimated_value = sum / no_of_paths;
    double variance =(sum_squared  - no_of_paths * estimated_value * estimated_value) /(no_of_paths - 1);
    double standardError = std::sqrt(variance / no_of_paths);

    return { estimated_value, standardError };
}