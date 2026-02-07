#include "cache_aware_module.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <algorithm>
#include <immintrin.h>

std::pair<double,double> CacheAwareModule::runSimulation(int no_of_paths,double spotPrice,double strikePrice,double timeToMaturity,double riskFreeRate,double volatility)
{
    std::vector<double> randomNumbers =generateRandomNormalVariables(no_of_paths);
    const double drift =(riskFreeRate - 0.5 * volatility * volatility) * timeToMaturity;
    const double diff = volatility * std::sqrt(timeToMaturity);
    const double discount = std::exp(-riskFreeRate * timeToMaturity);

    __m256d spot_v  = _mm256_set1_pd(spotPrice);
    __m256d drift_v = _mm256_set1_pd(drift);
    __m256d diff_v  = _mm256_set1_pd(diff);

    double sum = 0.0;
    double sum_squared  = 0.0;

    int i = 0;
    const int vecWidth = 4;

    alignas(32) double tmp[4];

    // -------- SIMD Loop --------
    for (; i + vecWidth <= no_of_paths; i += vecWidth)
    {
        __m256d z = _mm256_loadu_pd(&randomNumbers[i]);
        __m256d x = _mm256_fmadd_pd(diff_v, z, drift_v);
        _mm256_store_pd(tmp, x);

        for (int k = 0; k < 4; ++k)
        {
            double ST = spotPrice * std::exp(tmp[k]);
            double payoff = std::max(ST - strikePrice, 0.0);
            double discounted_payoff = discount * payoff;
            sum  += discounted_payoff;
            sum_squared  += discounted_payoff * discounted_payoff;
        }
    }

    for (; i < no_of_paths; ++i)
    {
        double ST = spotPrice * std::exp(drift + diff * randomNumbers[i]);
        double payoff =std::max(ST - strikePrice, 0.0);
        double discounted_payoff = discount * payoff;
        sum  += discounted_payoff;
        sum_squared  += discounted_payoff * discounted_payoff;
    }

    double estimated_value = sum / no_of_paths;
    double variance =(sum_squared  - no_of_paths * estimated_value * estimated_value) /(no_of_paths - 1);
    double standardError = std::sqrt(variance / no_of_paths);

    return { estimated_value, standardError };
}



// double CacheAwareModule::runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility){
//     std::vector<double> terminal_prices(no_of_paths); 
//     std::vector<double> randomNumbers = generateRandomNormalVariables(no_of_paths);  // could optimize this with memory alllginement ?

//     const double drift = (riskFreeRate - 0.5 * volatility * volatility) * timeToMaturity;
//     const double diff  = volatility * std::sqrt(timeToMaturity);

//     __m256d spot_v  = _mm256_set1_pd(spotPrice);
//     __m256d drift_v = _mm256_set1_pd(drift);
//     __m256d diff_v  = _mm256_set1_pd(diff);

//     int i = 0;
//     int vecWidth = 4;

//     for (; i + vecWidth <= no_of_paths; i += vecWidth) {
//         __m256d z = _mm256_loadu_pd(&randomNumbers[i]);          
//         __m256d x = _mm256_fmadd_pd(diff_v, z, drift_v);  

//         alignas(32) double tmp[4];
//         _mm256_store_pd(tmp, x);                          

//         for (int k = 0; k < 4; ++k) {
//             tmp[k] = std::exp(tmp[k]);                    
//         }

//         __m256d e = _mm256_load_pd(tmp);
//         __m256d S = _mm256_mul_pd(spot_v, e);             
//         _mm256_storeu_pd(&terminal_prices[i], S);         
//     }

//     for (; i < no_of_paths; ++i) {                                  
//         double z = randomNumbers[i];
//         terminal_prices[i] = spotPrice * std::exp(drift + diff * z);
//     }

//     // for (int i = 0;i<no_of_paths;i++) { //  vectoriziton ? loop unrolling ?
//     //     double z = randomNumbers[i];
//     //     terminal_prices[i] = spotPrice * std::exp(drift+diff*z);
//     //  }

//     double sum = simulatePayOffs(no_of_paths,terminal_prices,strikePrice); // optimize this ?

//     double estimated_value = std::exp(-riskFreeRate*timeToMaturity) * ( sum / no_of_paths);


//     // need to change to call functions and return the estimated value of option
//     return estimated_value;
// }


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

    


}


