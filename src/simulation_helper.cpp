#include "simulation_helper.h"
#include <chrono>
#include <algorithm>
#include <iostream>
#include <cmath>

SimulationHelper::SimulationHelper() {

}
SimulationHelper::~SimulationHelper() {
    StopSimulation();
}

void SimulationHelper::StartSimulation(const SimulationParams& params) {
        StopSimulation(); 
    
    {
        std::lock_guard<std::mutex> lock(resultsMutex);
        currentParams = params;
        currentResults = SimulationResults();
        currentResults.isComplete = false;
    }
    shouldStop.store(false);
    isRunning.store(true);
    progress.store(0.0);
    
    simThread = std::thread(&SimulationHelper::SimulationThread, this); // need to undsertand how this works more
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
    RunModel(currentParams);
    isRunning.store(false);
}

void SimulationHelper::RunModel(const SimulationParams& params) {

    std::vector<int64_t> times;
    std::vector<double> estimatedValues;
    std::vector<double> errors;
    double estimatedValue = 0.0;
    std::string modelName;
    
    // Warmup invocations
    for (int i = 0; i < 100; i++) {
        if (shouldStop.load()) {
            return;
        }
        
        switch (params.modelType) {
            case ModelType::VANILLA:
                vanillaEngine.runSimulation(params.numPaths, params.spotPrice, 
                    params.strikePrice, params.timeToMaturity, 
                    params.riskFreeRate, params.volatility);
                modelName = "Vanilla Monte Carlo Model";
                break;
            case ModelType::VARIANCE_REDUCTION:
                varianceEngine.runSimulation(params.numPaths, params.spotPrice, 
                    params.strikePrice, params.timeToMaturity, 
                    params.riskFreeRate, params.volatility);
                modelName = "Variance Reduction Model";
                break;
            case ModelType::CACHE_AWARE:
                cacheEngine.runSimulation(params.numPaths, params.spotPrice, 
                    params.strikePrice, params.timeToMaturity, 
                    params.riskFreeRate, params.volatility);
                modelName = "Cache Aware Model";
                break;
            case ModelType::BLACK_SCHOLES:
                {
                    BlackScholes bs(params.spotPrice, params.strikePrice, 
                        params.timeToMaturity, params.riskFreeRate, params.volatility);
                    bs.callPrice();
                    modelName = "Black-Scholes Model";
                }
                break;
        }
        
        progress.store(static_cast<double>(i) / 100.0 * 0.1); 
    }
    
    // benchmark runs
    const int NUM_RUNS = params.iterations;
    for (int i = 0; i < NUM_RUNS; i++) {
        if (shouldStop.load()) {
            return;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        switch (params.modelType) {
            case ModelType::VANILLA:
                estimatedValue = vanillaEngine.runSimulation(params.numPaths, 
                    params.spotPrice, params.strikePrice, params.timeToMaturity, 
                    params.riskFreeRate, params.volatility);
                break;
            case ModelType::VARIANCE_REDUCTION:
                estimatedValue = varianceEngine.runSimulation(params.numPaths, 
                    params.spotPrice, params.strikePrice, params.timeToMaturity, 
                    params.riskFreeRate, params.volatility);
                break;
            case ModelType::CACHE_AWARE:
                estimatedValue = cacheEngine.runSimulation(params.numPaths, 
                    params.spotPrice, params.strikePrice, params.timeToMaturity, 
                    params.riskFreeRate, params.volatility);
                break;
            case ModelType::BLACK_SCHOLES:
                {
                    BlackScholes bs(params.spotPrice, params.strikePrice, 
                        params.timeToMaturity, params.riskFreeRate, params.volatility);
                    estimatedValue = bs.callPrice();
                }
                break;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        times.push_back(duration);

        estimatedValues.push_back(estimatedValue);
        
        progress.store(0.1 + (static_cast<double>(i + 1) / NUM_RUNS) * 0.9);

        BlackScholes bsValueEngine(params.spotPrice, params.strikePrice, 
        params.timeToMaturity, params.riskFreeRate, params.volatility);
        double bsValue = bsValueEngine.callPrice();

        errors.push_back(fabs(estimatedValue - bsValue));

        if (params.iterations>=100){
                    // updating results every 10 iterations to avoid over locking - need to ensure its still accuarte and a risk we can take
        if (i % (NUM_RUNS/100) == 0 || i == NUM_RUNS - 1) {
            auto min_max__estimated_values_pair = std::minmax_element(estimatedValues.begin(), estimatedValues.end()); //could optimize this
            auto min_error = std::min_element(errors.begin(),errors.end());
            std::lock_guard<std::mutex> lock(resultsMutex);
            currentResults.estimatedValue = estimatedValue;
            currentResults.minEstimatedValue = *min_max__estimated_values_pair.first;
            currentResults.maxEstimatedValue = *min_max__estimated_values_pair.second;
            currentResults.bsValue = bsValue;
            currentResults.error = fabs(estimatedValue - bsValue);
            currentResults.minError = *min_error;
            currentResults.timings = times;
            currentResults.iterationsCompleted = i + 1;
            currentResults.modelName = modelName;
        }

        }else{
            auto min_max__estimated_values_pair = std::minmax_element(estimatedValues.begin(), estimatedValues.end()); //could optimize this
            auto min_error = std::min_element(errors.begin(),errors.end());
            std::lock_guard<std::mutex> lock(resultsMutex);
            currentResults.estimatedValue = estimatedValue;
            currentResults.minEstimatedValue = *min_max__estimated_values_pair.first;
            currentResults.maxEstimatedValue = *min_max__estimated_values_pair.second;
            currentResults.bsValue = bsValue;
            currentResults.error = fabs(estimatedValue - bsValue);
            currentResults.minError = *min_error;
            currentResults.timings = times;
            currentResults.iterationsCompleted = i + 1;
            currentResults.modelName = modelName;
       }
        

    }
    
    
    int64_t sum = 0;
    for (auto t : times) {
        sum += t;
    }
    double meanTime = static_cast<double>(sum) / times.size();
    int64_t minTime = *std::min_element(times.begin(), times.end());
    
    {
        // do i need to add min max pair here ?
        std::lock_guard<std::mutex> lock(resultsMutex);
        currentResults.estimatedValue = estimatedValue;
        currentResults.meanTime = meanTime;
        currentResults.minTime = minTime;
        currentResults.timings = std::move(times);
        currentResults.iterationsCompleted = NUM_RUNS;
        currentResults.isComplete = true;
        currentResults.modelName = modelName;
    }
    
    progress.store(1.0);
}