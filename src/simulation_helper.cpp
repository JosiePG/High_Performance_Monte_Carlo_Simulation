#include "simulation_helper.h"
#include <chrono>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>

SimulationHelper::SimulationHelper() {

}
SimulationHelper::~SimulationHelper() {
    StopSimulation();
}

void SimulationHelper::StartSimulation(const OptionParameters& OptParams, SimulationParams& SimParams,bool isConvergencePlot) {
        StopSimulation(); 
    
    {
        std::lock_guard<std::mutex> lock(resultsMutex); //
        currentOptParams = OptParams;
        currentSimParams = SimParams;
        currentResults = SimulationResults();
        currentResults.isComplete = false;
    }
    shouldStop.store(false);
    isRunning.store(true);
    progress.store(0.0);

    if(isConvergencePlot){
        simThread = std::thread(&SimulationHelper::ConvergencePlotThread, this);
    }else{
        simThread = std::thread(&SimulationHelper::SimulationThread, this); 

    }
    
    
}

void SimulationHelper::StopSimulation() {
    shouldStop.store(true);
    
    if (simThread.joinable()) {
        simThread.join();
    }
    
}

SimulationResults SimulationHelper::GetResults() const {
    std::lock_guard<std::mutex> lock(resultsMutex);
    return currentResults; 
}

void SimulationHelper::SimulationThread() {
    RunModel(currentOptParams,currentSimParams);
    isRunning.store(false);
}

 void SimulationHelper::ConvergencePlotThread(){
    RunConvergencePlot(currentOptParams,currentSimParams);
    isRunning.store(false);
 }

void SimulationHelper::RunModel(const OptionParameters& OptParams, SimulationParams& SimParams) {

    std::vector<int64_t> times;
    std::vector<double> estimatedValues;
    double min_value = std::numeric_limits<double>::max();
    double max_value = std::numeric_limits<double>::min();
    std::pair<double,double> result;
    double standardError = 0.0;
    double estimatedValue = 0.0;
    std::string modelName;
    
    BlackScholes bsValueEngine(OptParams.spotPrice, OptParams.strikePrice, 
    OptParams.timeToMaturity, OptParams.riskFreeRate, OptParams.volatility);
    double bsValue = (OptParams.optionType == OptionType::CALL)
                 ? bsValueEngine.callPrice()
                 : bsValueEngine.putPrice();
    
    // Warmup invocations
    for (int i = 0; i < 5; i++) {
        if (shouldStop.load()) {
            return;
        }
        
        switch (SimParams.modelType) {
            case ModelType::VANILLA:
                vanillaEngine.price(SimParams.numPaths, OptParams);
                modelName = "Vanilla Monte Carlo Model";
                break;
            case ModelType::VARIANCE_REDUCTION:
                varianceEngine.price(SimParams.numPaths, OptParams);
                modelName = "Variance Reduction Model";
                break;
            case ModelType::CACHE_AWARE:
                cacheEngine.price(SimParams.numPaths, OptParams);
                modelName = "Cache Aware Model";
                break;
            case ModelType::PARALLEL:
                parallelEngine.price(SimParams.numPaths, OptParams);
                modelName = "Parallel Model";
                break;
            case ModelType::ULTIMATE:
                ultimateEngine.price(SimParams.numPaths, OptParams);
                modelName = "Ultimate Model";
                break;
        }
        
        progress.store(static_cast<double>(i) / 100.0 * 0.1); 
    }
    
    // benchmark runs
    const int NUM_RUNS = SimParams.iterations;
    for (int i = 0; i < NUM_RUNS; i++) {
        if (shouldStop.load()) {
            return;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        switch (SimParams.modelType) {
            case ModelType::VANILLA:
                result = vanillaEngine.price(SimParams.numPaths, OptParams);
                break;
            case ModelType::VARIANCE_REDUCTION:
                result = varianceEngine.price(SimParams.numPaths, OptParams);
                break;
            case ModelType::CACHE_AWARE:
                result = cacheEngine.price(SimParams.numPaths, OptParams);
                break;
            case ModelType::PARALLEL:
                result = parallelEngine.price(SimParams.numPaths, OptParams);
                modelName = "Parallel Model";
                break;
            case ModelType::ULTIMATE:
                result = ultimateEngine.price(SimParams.numPaths, OptParams);
                modelName = "Ultimate Model";
                break;
        }

        estimatedValue = result.first;
        standardError = result.second;
        
        auto end = std::chrono::high_resolution_clock::now();
        int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        times.push_back(duration);

        estimatedValues.push_back(estimatedValue);
        if (estimatedValue < min_value) min_value = estimatedValue;
        if (estimatedValue > max_value) max_value = estimatedValue;

        
        
        progress.store(0.1 + (static_cast<double>(i + 1) / NUM_RUNS) * 0.9);

        if (SimParams.iterations>=100){
        // updating results every 10 iterations to avoid over locking 
        if (i % (NUM_RUNS/100) == 0 || i == NUM_RUNS - 1) {
            std::lock_guard<std::mutex> lock(resultsMutex);
            currentResults.estimatedValue = estimatedValue;
            currentResults.minEstimatedValue = min_value;
            currentResults.maxEstimatedValue = max_value;
            currentResults.bsValue = bsValue;
            currentResults.error = fabs(estimatedValue - bsValue);
            currentResults.standardError = standardError;
            currentResults.ci_hi = estimatedValue + (1.96 * standardError);
            currentResults.ci_lo = estimatedValue - (1.96 * standardError);
            currentResults.timings = times;
            currentResults.iterationsCompleted = i + 1;
            currentResults.modelName = modelName;
        }

        }else{
            std::lock_guard<std::mutex> lock(resultsMutex);
            currentResults.estimatedValue = estimatedValue;
            currentResults.minEstimatedValue = min_value;
            currentResults.maxEstimatedValue = max_value;
            currentResults.bsValue = bsValue;
            currentResults.error = fabs(estimatedValue - bsValue);
            currentResults.standardError = standardError;
            currentResults.ci_hi = estimatedValue + (1.96 * standardError);
            currentResults.ci_lo = estimatedValue - (1.96 * standardError);
            currentResults.timings = times;
            currentResults.iterationsCompleted = i + 1;
            currentResults.modelName = modelName;
       }
        

    }
    
    
    int64_t sum = 0;
    for (auto t : times) {
        sum += t;
    }
    double meanTime = times.empty() ? 0.0 : static_cast<double>(sum) / times.size();
    int64_t minTime = *std::min_element(times.begin(), times.end());
    
    {
        // does a final update of results to ensure everything is stored 
        auto min_max__estimated_values_pair = std::minmax_element(estimatedValues.begin(), estimatedValues.end());
        std::lock_guard<std::mutex> lock(resultsMutex);
        currentResults.estimatedValue = estimatedValue;
        currentResults.standardError = standardError;
        currentResults.minEstimatedValue = *min_max__estimated_values_pair.first;
        currentResults.maxEstimatedValue = *min_max__estimated_values_pair.second;
        currentResults.ci_hi = estimatedValue + (1.96 * standardError);
        currentResults.ci_lo = estimatedValue - (1.96 * standardError);
        currentResults.meanTime = meanTime;
        currentResults.minTime = minTime;
        currentResults.timings = times;
        currentResults.iterationsCompleted = NUM_RUNS;
        currentResults.modelName = modelName;
        currentResults.isComplete = true;
    }
    
    progress.store(1.0);
}

void SimulationHelper::RunConvergencePlot(const OptionParameters& OptParams, SimulationParams& SimParams) {
    std::vector<double> errors;
    std::vector<double> paths;
    std::pair<double,double> result;
    double standardError = 0.0;
    double estimatedValue = 0.0;
    std::string modelName;
    std::vector<int> numPaths = {
    1000, 2500, 5000, 10000, 25000, 50000, 100000, 250000, 500000, 1000000
};

    BlackScholes bs(OptParams.spotPrice, OptParams.strikePrice,OptParams.timeToMaturity, OptParams.riskFreeRate, OptParams.volatility);

    double bsValue = (OptParams.optionType == OptionType::CALL)
                 ? bs.callPrice()
                 : bs.putPrice();
    

    for(int path:numPaths){
        if (shouldStop.load()) {
            return;
        }
        
        
        switch (SimParams.modelType) {
            case ModelType::VANILLA:
                result = vanillaEngine.price(path, OptParams);
                break;
            case ModelType::VARIANCE_REDUCTION:
                result = varianceEngine.price(path, OptParams);
                break;
            case ModelType::CACHE_AWARE:
                result = cacheEngine.price(path, OptParams);
                break;
            case ModelType::PARALLEL:
                result = parallelEngine.price(SimParams.numPaths, OptParams);
                modelName = "Parallel Model";
                break;
            case ModelType::ULTIMATE:
                result = ultimateEngine.price(SimParams.numPaths, OptParams);
                modelName = "Ultimate Model";
                break;
        }

        estimatedValue = result.first;
        standardError = result.second;
        

        paths.push_back(static_cast<double>(path));
        errors.push_back(standardError);

        progress.store(static_cast<double>(paths.size()) / numPaths.size());


        std::lock_guard<std::mutex> lock(resultsMutex);
        currentResults.convergencePaths = paths;
        currentResults.convergenceSE = errors;
        currentResults.isComplete = true;
        currentResults.modelName = modelName;
       
    }

    progress.store(1.0);

}