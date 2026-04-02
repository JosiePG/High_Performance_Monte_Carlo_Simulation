
#ifndef MONTE_CARLO_SIMULATION_H
#define MONTE_CARLO_SIMULATION_H

#include <cmath>
#include <utility>
#include <string>
#include "option_parameters.h"
#include "rng_policies.h"
#include "sampling_policies.h"
#include "exec_policies.h"
#include "black_scholes_model.h"
 
// =============================================================================
// IMonteCarloEngine
//
// A simple abstract base class so that different engine configurations
// can be stored together and called through a common interface.
//
// The virtual call happens ONCE per simulation (not once per path),
// so there is no performance cost inside the hot loop.
// =============================================================================
class IMonteCarloEngine {
public:
    virtual std::pair<double, double> price(int numberOfPaths, const OptionParameters& params) = 0;
    virtual ~IMonteCarloEngine() = default;
};
 
 
// =============================================================================
// MonteCarloEngine<RngPolicy, SamplingPolicy, ExecutionPolicy>
//
// The composable engine. You choose one policy from each category:
//
//   RngPolicy       — how random numbers are generated
//   SamplingPolicy  — plain paths or antithetic variance reduction
//   ExecutionPolicy — serial / cache-aware AVX2 / OpenMP / OpenMP+AVX2
//
// All three policies are resolved at COMPILE TIME — the compiler sees
// through every policy call and can inline and optimise them fully.
// There is zero virtual dispatch or overhead inside the simulation loop.
//


template<
    typename RngPolicy,
    typename SamplingPolicy,
    typename ExecutionPolicy
>
class MonteCarloEngine : public IMonteCarloEngine {
 
    RngPolicy       rngPolicy;
    SamplingPolicy  samplingPolicy;
    ExecutionPolicy executionPolicy;
 
public:
    MonteCarloEngine(){}
 
 
    std::pair<double, double> price(int numberOfPaths, const OptionParameters& params) override {
 
        double totalSum        = 0.0;
        double totalSumSquared = 0.0;
 
        // Pre-compute constants that are the same for every path
        // Doing this outside the loop avoids repeated multiplications
        double precomputedDrift    = (params.riskFreeRate - 0.5 * params.volatility * params.volatility)
                                      * params.timeToMaturity;
        double precomputedVolSqrtT = params.volatility * std::sqrt(params.timeToMaturity);
        double discountFactor      = std::exp(-params.riskFreeRate * params.timeToMaturity);
 
        // Hand off to whichever execution policy was chosen at compile time
        executionPolicy.runPaths(numberOfPaths, params,
                                  precomputedDrift, precomputedVolSqrtT, discountFactor,
                                  rngPolicy, samplingPolicy,
                                  totalSum, totalSumSquared);
 
        // Compute the final price estimate and its standard error
        double estimatedPrice = totalSum / numberOfPaths;
        double variance       = (totalSumSquared - numberOfPaths * estimatedPrice * estimatedPrice)
                                 / (numberOfPaths - 1);
        double standardError  = std::sqrt(variance / numberOfPaths);
 
        return std::make_pair(estimatedPrice, standardError);
    }

        // Returns the Black-Scholes analytical price for comparison
    double blackScholesPrice(const OptionParameters& params) {
        BlackScholes analyticalModel(params.spotPrice, params.strikePrice,
                                     params.timeToMaturity, params.riskFreeRate,
                                     params.volatility);
        return (params.optionType == OptionType::CALL)
           ? analyticalModel.callPrice()
           : analyticalModel.putPrice();
    }
};


#endif 