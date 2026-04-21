
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
 
// Abstract base class (inerface) for any Monte Carlo engine
class IMonteCarloEngine {
public:
    virtual std::pair<double, double> price(int numberOfPaths, const OptionParameters& params) = 0;
    virtual ~IMonteCarloEngine() = default;
};
 

// Templated Monte Carlo engine using policy-based design
// RngPolicy: how random numbers are generated
// SamplingPolicy: how paths/payoffs are sampled 
// ExecutionPolicy: how computation is executed 

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
 
        // Accumulators for Monte Carlo estimation
        double totalSum        = 0.0; // Sum of discounted payoffs
        double totalSumSquared = 0.0; // Sum of squared payoffs (for variance calculation)
 
        // Pre-compute constants that are the same for every path
        // Doing this outside the loop avoids repeated multiplications
        double precomputedDrift    = (params.riskFreeRate - 0.5 * params.volatility * params.volatility)
                                      * params.timeToMaturity; // Drift term: (r - 0.5 * sigma^2) * T

        double precomputedVolSqrtT = params.volatility * std::sqrt(params.timeToMaturity); // Volatility scaling: sigma * sqrt(T)

        double discountFactor      = std::exp(-params.riskFreeRate * params.timeToMaturity);// Discount factor: exp(-rT)
 
        // Hands off to whichever execution policy was chosen at compile time
        executionPolicy.runPaths(numberOfPaths, params,
                                  precomputedDrift, precomputedVolSqrtT, discountFactor,
                                  rngPolicy, samplingPolicy,
                                  totalSum, totalSumSquared);
 
        // Computes the final price estimate and its standard error
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