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

// Enum representing which Monte Carlo model to run
enum class ModelType {
    VANILLA = 0,
    VARIANCE_REDUCTION = 1,
    CACHE_AWARE = 2,
    PARALLEL = 3,
    ULTIMATE = 4,
};

// Input parameters controlling simulation behaviour
struct SimulationParams {
    int numPaths;
    ModelType modelType;
    int iterations;
};

// struct for storing summary data (to display on the UI)
    struct SimulationData{
    std::string model;
    std::string optionType;
    int         numPaths;       
    int         iterations;      
    double std_error;
    double mean_time;
    };


            
// Contains comprehensive simualtion results (thread-safe access required)
struct SimulationResults {
    double estimatedValue; // Final MC price
    double minEstimatedValue; // Min estimate across runs
    double maxEstimatedValue; // Max estimate across runs
    double bsValue; // Analytical Black-Scholes price
    double error; // Absolute error
    double standardError;
    double ci_hi; // Upper 95% confidence bound
    double ci_lo;  // Lower 95% confidence bound
    double meanTime; // Average runtime
    double minTime; // Fastest runtime
    std::vector<int64_t> timings;
    int iterationsCompleted;
    bool isComplete;
    std::string modelName;
    std::vector<double> convergencePaths;
    std::vector<double> convergenceSE;
};

// Helper class that manages simulation lifecycle (threading + results)
class SimulationHelper {
public:
    SimulationHelper();
    ~SimulationHelper();
    
    // Starts simulation in a background thread
    void StartSimulation(const OptionParameters& OptParams, SimulationParams& SimParams,bool isConvergencePlot);
    void StopSimulation();
    
    bool IsRunning() const { return isRunning.load(); }

    SimulationResults GetResults() const;
    double GetProgress() const { return progress.load(); }
    
private:
    // Worker threads
    void SimulationThread();  // Standard benchmark execution
    void ConvergencePlotThread();

    void RunModel(const OptionParameters& OptParams, SimulationParams& SimParams);
    void RunConvergencePlot(const OptionParameters& OptParams, SimulationParams& SimParams);
    
    
    std::thread simThread;
    // Atomic flags for safe cross-thread communication
    std::atomic<bool> isRunning{false};
    std::atomic<bool> shouldStop{false};
    std::atomic<double> progress{0.0};
    mutable std::mutex resultsMutex; // Mutex to protect shared results data
    SimulationResults currentResults;
    OptionParameters currentOptParams;
    SimulationParams currentSimParams;
    
     // Pre-instantiated engines
    MonteCarloEngine<StandardMersenneTwisterGenerator,
                     PlainMonteCarloSampling,
                     SerialExecutionPolicy> 
                     vanillaEngine;
    MonteCarloEngine<StandardMersenneTwisterGenerator,
                     AntitheticVariateSampling,
                     SerialExecutionPolicy>
                     varianceEngine;
    MonteCarloEngine<XoshiroSingleThreadedGenerator,
                 PlainMonteCarloSampling,
                 CacheAwareVectorizedExecutionPolicy>
                     cacheEngine;
MonteCarloEngine<XoshiroPerThreadGenerator,
                 PlainMonteCarloSampling,
                 OpenMPParallelExecutionPolicy>
                     parallelEngine;
     MonteCarloEngine<XoshiroPerThreadGenerator,
                     AntitheticVariateSampling,
                     OpenMPParallelExecutionPolicy>
                     ultimateEngine;
};


#endif