#include <algorithm>
#include "parallel_mc_engine.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <immintrin.h>
#include <vector>
#include "xoshiro.h"
#include <omp.h>


std::pair<double,double> ParallelMcEngine::runSimulation(
    int no_of_paths, double spotPrice, double strikePrice,
    double timeToMaturity, double riskFreeRate, double volatility)
{
    double sum         = 0.0;
    double sum_squared = 0.0;

    const double discount_factor = std::exp(-riskFreeRate * timeToMaturity);
    const double drift           = (riskFreeRate - 0.5 * volatility * volatility) * timeToMaturity;
    const double vol_sqrt_t      = volatility * std::sqrt(timeToMaturity);

    // Setup per-thread RNGs BEFORE the parallel region
    const int max_threads = omp_get_max_threads();
    std::vector<std::mt19937_64> generators(max_threads);
    std::vector<std::normal_distribution<double>> dists(
        max_threads, std::normal_distribution<double>(0.0, 1.0));

    std::random_device rd;
    for (int t = 0; t < max_threads; ++t)
        generators[t].seed(rd());

    // Single parallel region — generate AND simulate together
    // No O(N) allocation, no second memory pass
    #pragma omp parallel for schedule(static) reduction(+:sum, sum_squared)
    for (int i = 0; i < no_of_paths; ++i) {
        const int tid = omp_get_thread_num();

        double z             = dists[tid](generators[tid]);   // inline generation
        double terminal_price = spotPrice * std::exp(drift + vol_sqrt_t * z);
        double disc_payoff    = discount_factor * calculatePayOff(terminal_price, strikePrice);

        sum         += disc_payoff;
        sum_squared += disc_payoff * disc_payoff;
    }

    double estimated_value = sum / no_of_paths;
    double variance        = (sum_squared - no_of_paths * (estimated_value * estimated_value))
                             / (no_of_paths - 1);
    double standard_error  = std::sqrt(variance / no_of_paths);

    return {estimated_value, standard_error};
}
