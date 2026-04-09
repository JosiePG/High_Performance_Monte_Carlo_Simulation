#ifndef SAMPLING_POLICIES_H
#define SAMPLING_POLICIES_H
#include <cmath>
#include <algorithm>
#include "option_parameters.h"

struct PlainMonteCarloSampling {
 
    void accumulatePath(double discountFactor,
                        double randomVariate,
                        const OptionParameters& params,
                        double precomputedDrift,
                        double precomputedVolSqrtT,
                        double& runningSum,
                        double& runningSumSquared) const {
 
        double terminalPrice   = params.spotPrice * std::exp(precomputedDrift + precomputedVolSqrtT * randomVariate);
        double payoff = (params.optionType == OptionType::CALL)
                      ? std::max(terminalPrice - params.strikePrice, 0.0)
                      : std::max(params.strikePrice - terminalPrice, 0.0);
        double discountedPayoff = discountFactor * payoff;
 
        runningSum        += discountedPayoff;
        runningSumSquared += discountedPayoff * discountedPayoff;
    }
};


 
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

        double payoff1, payoff2;
        if (params.optionType == OptionType::CALL) {
            payoff1 = std::max(terminalPrice1 - params.strikePrice, 0.0);
            payoff2 = std::max(terminalPrice2 - params.strikePrice, 0.0);
        } else {
            payoff1 = std::max(params.strikePrice - terminalPrice1, 0.0);
            payoff2 = std::max(params.strikePrice - terminalPrice2, 0.0);
        }
 
        // Average the two payoffs — this is what reduces variance
        double averagePayoff   = 0.5 * discountFactor * (payoff1 + payoff2);
 
        runningSum        += averagePayoff;
        runningSumSquared += averagePayoff * averagePayoff;
    }
};

#endif