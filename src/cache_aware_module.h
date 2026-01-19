#include "monte_carlo_engine.h"

#ifndef FINAL_YEAR_PROJECT_CACHE_AWARE_MODULE_H
#define FINAL_YEAR_PROJECT_CACHE_AWARE_MODULE_H


class  CacheAwareModule : public MonteCarloEngine {
private:
    size_t cacheLineSize;
public:
    double runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility) override;
    void allocateAllignedMemory(size_t size);
    double vectorizedPayOff();

};


#endif //FINAL_YEAR_PROJECT_CACHE_AWARE_MODULE_H