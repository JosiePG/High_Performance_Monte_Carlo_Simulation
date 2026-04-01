#ifndef SAMPLING_POLICIES_H
#define SAMPLING_POLICIES_H
#include <cmath>
#include <algorithm>
#include "option_parameters.h"
 
// =============================================================================
// POLICY: PlainMonteCarloSampling
//
// The standard Monte Carlo approach.
// Each random variate z produces one simulated asset price via the
// Geometric Brownian Motion (GBM) formula:
//
//   S_T = S * exp( (r - 0.5*sigma^2)*T  +  sigma*sqrt(T)*z )
//
// The payoff is then:  max(S_T - K, 0)
// =============================================================================
struct PlainMonteCarloSampling {
 
    void accumulatePath(double discountFactor,
                        double randomVariate,
                        const OptionParameters& params,
                        double precomputedDrift,
                        double precomputedVolSqrtT,
                        double& runningSum,
                        double& runningSumSquared) const {
 
        double terminalPrice   = params.spotPrice * std::exp(precomputedDrift + precomputedVolSqrtT * randomVariate);
        double payoff          = std::max(terminalPrice - params.strikePrice, 0.0);
        double discountedPayoff = discountFactor * payoff;
 
        runningSum        += discountedPayoff;
        runningSumSquared += discountedPayoff * discountedPayoff;
    }
};
 
 
// =============================================================================
// POLICY: AntitheticVariateSampling
//
// A variance reduction technique.
// For each random variate z, also simulate a path using -z (the antithetic).
// These two paths are negatively correlated, so averaging their payoffs
// cancels out some of the Monte Carlo noise.
//
// Result: same number of RNG calls, but roughly half the variance.
// The standard error therefore falls faster with fewer paths.
// =============================================================================
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
        double payoff1         = std::max(terminalPrice1 - params.strikePrice, 0.0);
 
        // Antithetic path using -z
        double terminalPrice2  = params.spotPrice * std::exp(precomputedDrift + precomputedVolSqrtT * -randomVariate);
        double payoff2         = std::max(terminalPrice2 - params.strikePrice, 0.0);
 
        // Average the two payoffs — this is what reduces variance
        double averagePayoff   = 0.5 * discountFactor * (payoff1 + payoff2);
 
        runningSum        += averagePayoff;
        runningSumSquared += averagePayoff * averagePayoff;
    }
};

#endif