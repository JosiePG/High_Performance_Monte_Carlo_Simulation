#ifndef FINAL_YEAR_PROJECT_SIMULATION_HELPER_H
#define FINAL_YEAR_PROJECT_SIMULATION_HELPER_H

#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <queue>
#include "monte_carlo_engine.h"
#include "variance_reduction_module.h"
#include "cache_aware_module.h"
#include "black_scholes_model.h"

enum class ModelType {
    VANILLA,
    VARIANCE_REDUCTION,
    CACHE_AWARE,
    BLACK_SCHOLES
};

struct SimulationParams {
    double spotPrice;
    double strikePrice;
    double timeToMaturity;
    double riskFreeRate;
    double volatility;
    int numPaths;
    ModelType modelType;
};

struct SimulationResults {
    double estimatedValue;
    double bsValue;
    double error;
    double meanTime;
    double minTime;
    std::vector<int64_t> timings;
    uint64_t iterationsCompleted;
    bool isComplete;
    std::string modelName;
};

class SimulationHelper {
public:
    SimulationHelper();
    ~SimulationHelper();
    
    void StartSimulation(const SimulationParams& params);
    void StopSimulation();
    
    bool IsRunning() const { return isRunning.load(); }
    SimulationResults GetResults() const;
    double GetProgress() const { return progress.load(); }
    
private:
    void SimulationThread();
    void RunModel(const SimulationParams& params);
    
    std::thread simThread;
    std::atomic<bool> isRunning{false};
    std::atomic<bool> shouldStop{false};
    std::atomic<double> progress{0.0};
    
    mutable std::mutex resultsMutex;
    SimulationResults currentResults;
    SimulationParams currentParams;
    
    
    
    MonteCarloEngine vanillaEngine;
    VarianceReductionModule varianceEngine;
    CacheAwareModule cacheEngine;
};


#endif