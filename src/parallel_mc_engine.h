#include "monte_carlo_engine.h"

#ifndef PARALLEL_MC_ENGINE_H
#define PARALLEL_MC_ENGINE_H


class  ParallelMcEngine : public MonteCarloEngine {
public:
    std::pair<double, double> runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility) override;
};


#endif