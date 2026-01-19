#include "cache_aware_module.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>


double CacheAwareModule::runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility){
    clock_t start,end;
    start = clock();
    std::vector<double> terminal_prices(no_of_paths); // allign this in memory ? 
    std::vector<double> randomNumbers = generateRandomNormalVariables(no_of_paths);  // could optimize this with memory alllginement ?

    for (int i = 0;i<no_of_paths;i++) { //  vectoriziton ? loop unrolling ?

        terminal_prices[i] = spotPrice * std::exp(
            (riskFreeRate - 0.5 * std::pow(volatility,2.0))
            * timeToMaturity + volatility*std::sqrt(timeToMaturity)*randomNumbers[i]);
     }

    double sum = simulatePayOffs(no_of_paths,terminal_prices,strikePrice); // optimize this ?

    double estimated_value = std::exp(-riskFreeRate*timeToMaturity) * (1.0/no_of_paths) * sum;
    // why is clock time different for compilers
    // clock is not accurate need to think about threads
    end = clock();
    double time_taken_in_secs = double(end - start) / double(CLOCKS_PER_SEC);
    double time_taken_cpu_ticks = double(end - start);
    std::cout << "---------- Cache aware module time stats---------- " << std::endl;
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Time taken by program is : " << time_taken_in_secs ;
    std::cout << " sec " << std::endl;
    std::cout << "Time taken by program is : " << time_taken_cpu_ticks;
    std::cout << " cpu clock ticks " << std::endl;


    // need to change to call functions and return the estimated value of option
    return estimated_value;
}

void CacheAwareModule::allocateAllignedMemory(size_t size){

}

double CacheAwareModule::vectorizedPayOff(){
    return 1.0;

}