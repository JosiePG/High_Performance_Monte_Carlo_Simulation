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
#include "optimized_mc_engine.h"
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
    int iterations;
};

    struct SimulationData{
    std::string model;
    double std_error;
    double mean_time;
    double min_time;
    };


            

struct SimulationResults {
    double estimatedValue;
    double minEstimatedValue;
    double maxEstimatedValue;
    double bsValue;
    double error;
    double standardError;
    double ci_hi;
    double ci_lo;
    double meanTime;
    double minTime;
    std::vector<int64_t> timings;
    int iterationsCompleted;
    bool isComplete;
    std::string modelName;
    std::vector<double> convergencePaths;
    std::vector<double> convergenceSE;
};

class SimulationHelper {
public:
    SimulationHelper();
    ~SimulationHelper();
    
    void StartSimulation(const SimulationParams& params,bool isConvergencePlot);
    void StopSimulation();
    
    bool IsRunning() const { return isRunning.load(); }
    SimulationResults GetResults() const;
    double GetProgress() const { return progress.load(); }
    
private:
    void SimulationThread();
    void ConvergencePlotThread();
    void RunModel(const SimulationParams& params);
    void RunConvergencePlot(const SimulationParams& params);
    
    
    std::thread simThread;
    std::atomic<bool> isRunning{false};
    std::atomic<bool> shouldStop{false};
    std::atomic<double> progress{0.0};
    
    mutable std::mutex resultsMutex;
    SimulationResults currentResults;
    SimulationParams currentParams;
    
    
    
    MonteCarloEngine vanillaEngine;
    VarianceReductionModule varianceEngine;
    OptimizedMcEngine cacheEngine;
};


#endif