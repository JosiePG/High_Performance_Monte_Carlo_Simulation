#ifndef SAMPLING_POLICIES_H
#define SAMPLING_POLICIES_H
#include <cmath>
#include <algorithm>
#include "option_parameters.h"

// Standard Monte Carlo sampling, simulates one path per random draw
struct PlainMonteCarloSampling {
 
    void accumulatePath(double discountFactor,
                        double randomVariate,
                        const OptionParameters& params,
                        double precomputedDrift,
                        double precomputedVolSqrtT,
                        double& runningSum,
                        double& runningSumSquared) const {
 
        // Simulates terminal asset price under geometric Brownian motion
        double terminalPrice   = params.spotPrice * std::exp(precomputedDrift + precomputedVolSqrtT * randomVariate);// S_T = S_0 * exp((r - 0.5*sigma^2)*T + sigma*sqrt(T)*Z)
        // Computes payoff depending on option type
        double payoff = (params.optionType == OptionType::CALL)
                      ? std::max(terminalPrice - params.strikePrice, 0.0)
                      : std::max(params.strikePrice - terminalPrice, 0.0);
        // Discounst payoff back to present value
        double discountedPayoff = discountFactor * payoff;// exp(-rT) * payoff
 
        // Accumulates results for mean and variance estimation
        runningSum        += discountedPayoff;
        runningSumSquared += discountedPayoff * discountedPayoff;
    }
};


// Antithetic Variate Monte Carlo sampling, for each random draw Z, -Z is also used
struct AntitheticVariateSampling {
 
    void accumulatePath(double discountFactor,
                        double randomVariate,
                        const OptionParameters& params,
                        double precomputedDrift,
                        double precomputedVolSqrtT,
                        double& runningSum,
                        double& runningSumSquared) const {
 
        // Original path using +z
        double terminalPrice1  = params.spotPrice * std::exp(precomputedDrift + precomputedVolSqrtT *  randomVariate);
 
        // Antithetic path using -z
        double terminalPrice2  = params.spotPrice * std::exp(precomputedDrift + precomputedVolSqrtT * -randomVariate);

        // Computes both payoffs
        double payoff1, payoff2;
        if (params.optionType == OptionType::CALL) {
            payoff1 = std::max(terminalPrice1 - params.strikePrice, 0.0);
            payoff2 = std::max(terminalPrice2 - params.strikePrice, 0.0);
        } else {
            payoff1 = std::max(params.strikePrice - terminalPrice1, 0.0);
            payoff2 = std::max(params.strikePrice - terminalPrice2, 0.0);
        }
 
        // Averages the two payoffs, this reduces variance because errors in +Z and -Z tend to cancel out
        double averagePayoff   = 0.5 * discountFactor * (payoff1 + payoff2);
 
         // Accumulates averaged result
        runningSum        += averagePayoff;
        runningSumSquared += averagePayoff * averagePayoff;
    }
};

#endif