#include "monte_carlo_engine.h"

#ifndef FINAL_YEAR_PROJECT_VARIANCE_REDUCTION_MODULE_H
#define FINAL_YEAR_PROJECT_VARIANCE_REDUCTION_MODULE_H


class VarianceReductionModule : public MonteCarloEngine {
private :
    double blackScholesPrice(double spotPrice,double strikePrice,double optionExpiration,double riskFreeRate,double volatility );
public:
    // ~VarianceReductionModule() = default;
    std::pair<double,double> runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility) override;

    void benchmark(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility) override;

};


#endif //FINAL_YEAR_PROJECT_VARIANCE_REDUCTION_MODULE_H