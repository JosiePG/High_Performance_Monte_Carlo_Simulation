#include "monte_carlo_engine.h"

#ifndef VARIANCE_REDUCTION_MODULE_H
#define VARIANCE_REDUCTION_MODULE_H


class VarianceReductionModule : public MonteCarloEngine {
private :
    double blackScholesPrice(double spotPrice,double strikePrice,double optionExpiration,double riskFreeRate,double volatility );
public:

    std::pair<double,double> runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility) override;

};


#endif