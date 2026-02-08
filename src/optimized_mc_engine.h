#include "monte_carlo_engine.h"

#ifndef OPTIMIZED_MC_ENGINE_H
#define OPTIMIZED_MC_ENGINE_H


class  OptimizedMcEngine : public MonteCarloEngine {
public:
    std::vector<double> generateRandomNormalVariables(int no_of_paths) override;
    std::pair<double, double> runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility) override;
};


#endif