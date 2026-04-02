#ifndef FINAL_YEAR_PROJECT_SIMULATION_HELPER_H
#define FINAL_YEAR_PROJECT_SIMULATION_HELPER_H

#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <string>  
#include <queue>
#include "option_parameters.h"
#include "monte_carlo_engine.h"
#include "black_scholes_model.h"

enum class ModelType {
    VANILLA = 0,
    VARIANCE_REDUCTION = 1,
    CACHE_AWARE = 2,
    PARALLEL = 3,
    PARALLEL_CACHE = 4,
    ULTIMATE = 5,
};

struct SimulationParams {
    int numPaths;
    ModelType modelType;
    int iterations;
};

    struct SimulationData{
    std::string model;
    std::string optionType;
    int         numPaths;       
    int         iterations;      
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
    
    void StartSimulation(const OptionParameters& OptParams, SimulationParams& SimParams,bool isConvergencePlot);
    void StopSimulation();
    
    bool IsRunning() const { return isRunning.load(); }
    SimulationResults GetResults() const;
    double GetProgress() const { return progress.load(); }
    
private:
    void SimulationThread();
    void ConvergencePlotThread();
    void RunModel(const OptionParameters& OptParams, SimulationParams& SimParams);
    void RunConvergencePlot(const OptionParameters& OptParams, SimulationParams& SimParams);
    
    
    std::thread simThread;
    std::atomic<bool> isRunning{false};
    std::atomic<bool> shouldStop{false};
    std::atomic<double> progress{0.0};
    
    mutable std::mutex resultsMutex;
    SimulationResults currentResults;
    OptionParameters currentOptParams;
    SimulationParams currentSimParams;
    
    MonteCarloEngine<StandardMersenneTwisterGenerator,
                     PlainMonteCarloSampling,
                     SerialExecutionPolicy> 
                     vanillaEngine;
    MonteCarloEngine<StandardMersenneTwisterGenerator,
                     AntitheticVariateSampling,
                     SerialExecutionPolicy>
                     varianceEngine;
    MonteCarloEngine<XoshiroSingleThreadedGenerator,
                 EfficentMonteCarloSampling,
                 CacheAwareVectorizedExecutionPolicy>
                     cacheEngine;
MonteCarloEngine<XoshiroPerThreadGenerator,
                 EfficentMonteCarloSampling,
                 OpenMPParallelExecutionPolicy>
                     parallelEngine;
MonteCarloEngine<XoshiroPerThreadGenerator,
                 EfficentMonteCarloSampling,
                 OpenMPWithAVX2ExecutionPolicy>
                     parallelAndCacheEngine;
     MonteCarloEngine<XoshiroPerThreadGenerator,
                     AntitheticVariateSampling,
                     OpenMPWithAVX2ExecutionPolicy>
                     ultimateEngine;
};


#endif