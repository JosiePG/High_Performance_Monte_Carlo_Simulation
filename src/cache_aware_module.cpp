#include "cache_aware_module.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <algorithm>



double CacheAwareModule::runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility){
    std::vector<double> terminal_prices(no_of_paths); // allign this in memory ? 
    std::vector<double> randomNumbers = generateRandomNormalVariables(no_of_paths);  // could optimize this with memory alllginement ?

    for (int i = 0;i<no_of_paths;i++) { //  vectoriziton ? loop unrolling ?

        terminal_prices[i] = spotPrice * std::exp(
            (riskFreeRate - 0.5 * std::pow(volatility,2.0))
            * timeToMaturity + volatility*std::sqrt(timeToMaturity)*randomNumbers[i]);
     }

    double sum = simulatePayOffs(no_of_paths,terminal_prices,strikePrice); // optimize this ?

    double estimated_value = std::exp(-riskFreeRate*timeToMaturity) * (1.0/no_of_paths) * sum;


    // need to change to call functions and return the estimated value of option
    return estimated_value;
}

void CacheAwareModule::allocateAllignedMemory(size_t size){

}

double CacheAwareModule::vectorizedPayOff(){
    return 1.0;

}


void CacheAwareModule::benchmark(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility){
    
    std::vector<int64_t> times ;
    
    if(!std::chrono::high_resolution_clock::is_steady){
        std::cout << "Clock not steady" << std::endl;
    }

    for (int i = 0; i< 100; i++){// warump  
        CacheAwareModule::runSimulation(no_of_paths,spotPrice , strikePrice ,timeToMaturity, riskFreeRate , volatility);
    } 
	
	for (int i = 0; i< 1000; i++){// subject to change , have runs be a parameter 
        auto start_time = std::chrono::high_resolution_clock::now();
        CacheAwareModule::runSimulation(no_of_paths,spotPrice , strikePrice ,timeToMaturity, riskFreeRate , volatility);
        auto end_time = std::chrono::high_resolution_clock::now();

        int64_t time_in_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        times.push_back(time_in_microseconds);
    }   
    std::cout << "---------- Cache Aware Module Time stats---------- " << std::endl;
    std::cout << std::fixed << std::setprecision(6);
    //TODO : make stats header 
    std::vector<int64_t>::iterator min_value = std::min_element(times.begin(),times.end());
    int64_t sum = 0;
    for(auto time:times){
        sum += time;
    }
    double mean = static_cast<double>(sum) / static_cast<double>(times.size());
    std::cout << "Mean Time taken by program is : " << mean << " micro seconds " << std::endl;
    std::cout << "Min Time taken by program is : " << *min_value << " micro seconds " << std::endl;
    // can add more stats like std etc

    


}


