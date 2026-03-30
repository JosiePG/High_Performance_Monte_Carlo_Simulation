#include <algorithm>
#include "parallel_mc_engine.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <immintrin.h>
#include <vector>
#include "xoshiro.h"
#include <omp.h>

std::vector<double> ParallelMcEngine::generateRandomNormalVariables(int no_of_paths) {
    std::vector<double> randomNumbers(no_of_paths);

    // One RNG per thread, seeded independently
    const int max_threads = omp_get_max_threads();
    std::vector<std::mt19937_64> generators(max_threads);

    std::random_device rd;
    for (int t = 0; t < max_threads; ++t)
        generators[t].seed(rd());  // independent seed per thread

    std::normal_distribution<double> normal_dist(0.0, 1.0);
    // NOTE: normal_dist is stateless between calls when passed an rng,
    // but to be safe give each thread its own copy
    std::vector<std::normal_distribution<double>> dists(max_threads,
                                                         std::normal_distribution<double>(0.0, 1.0));

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < no_of_paths; ++i) {
        int tid = omp_get_thread_num();
        randomNumbers[i] = dists[tid](generators[tid]);
    }

    return randomNumbers;
}

std::pair<double,double> ParallelMcEngine::runSimulation(
    int no_of_paths, double spotPrice, double strikePrice,
    double timeToMaturity, double riskFreeRate, double volatility)
{
    std::vector<double> randomNumbers = generateRandomNormalVariables(no_of_paths);

    double sum         = 0.0;
    double sum_squared = 0.0;
    const double discount_factor = std::exp(-riskFreeRate * timeToMaturity);

    const double drift  = (riskFreeRate - 0.5 * volatility * volatility) * timeToMaturity;
    const double vol_sq = volatility * std::sqrt(timeToMaturity);

    #pragma omp parallel for schedule(static) \
            reduction(+:sum, sum_squared)
    for (int i = 0; i < no_of_paths; ++i) {
        double terminal_price = spotPrice * std::exp(drift + vol_sq * randomNumbers[i]);

        double discounted_payoff = discount_factor * calculatePayOff(terminal_price, strikePrice);

        sum         += discounted_payoff;
        sum_squared += discounted_payoff * discounted_payoff;
    }

    double estimated_value = sum / no_of_paths;
    double variance        = (sum_squared - no_of_paths * (estimated_value * estimated_value))
                             / (no_of_paths - 1);
    double standard_error  = std::sqrt(variance / no_of_paths);

    return {estimated_value, standard_error};
}