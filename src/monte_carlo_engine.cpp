#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include "monte_carlo_engine.h"

std::vector<double> MonteCarloEngine::generateRandomNormalVariables(int no_of_paths) {

    std::vector<double> randomNumbers(no_of_paths);

    std::random_device rd; // generates random seed
    std::default_random_engine generator(rd()); // generates random numbers
    std::normal_distribution<double> normal_dist(0.0,1);

    for (int i=0;i<no_of_paths;i++) {
        randomNumbers[i] = normal_dist(generator);
    }


    return randomNumbers;

}

double MonteCarloEngine::calculatePayOff(double terminal_price,double strike_price) {
    double payOff = std::max((terminal_price - strike_price),0.0);
    return payOff;
}

double MonteCarloEngine::simulatePayOffs(int no_of_paths,std::vector<double> terminal_prices,double strike_price) {
    double sum_of_pay_offs = 0.0;
    for (int i=0;i<no_of_paths;i++) {
        sum_of_pay_offs+= calculatePayOff(terminal_prices[i],strike_price);
    }

    return sum_of_pay_offs;
}

double MonteCarloEngine::runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility) {
    std::vector<double> terminal_prices(no_of_paths);
    std::vector<double> randomNumbers = generateRandomNormalVariables(no_of_paths);

    for (int i = 0;i<no_of_paths;i++) {

        terminal_prices[i] = spotPrice * std::exp(
            (riskFreeRate - 0.5 * std::pow(volatility,2.0))
            * timeToMaturity + volatility*std::sqrt(timeToMaturity)*randomNumbers[i]);
     }

    double sum = simulatePayOffs(no_of_paths,terminal_prices,strikePrice);

    double estimated_value = std::exp(-riskFreeRate*timeToMaturity) * (1.0/no_of_paths) * sum;

    // need to change to call functions and return the estimated value of option
    return estimated_value;
}

void MonteCarloEngine::benchmark(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility){
    
    std::vector<int64_t> times ;
    
    if(!std::chrono::high_resolution_clock::is_steady){
        std::cout << "Clock not steady" << std::endl;
    }

    for (int i = 0; i< 100; i++){// warump  
        MonteCarloEngine::runSimulation(no_of_paths,spotPrice , strikePrice ,timeToMaturity, riskFreeRate , volatility);
    } 
	
	for (int i = 0; i< 1000; i++){// subject to change , have runs be a parameter 
        auto start_time = std::chrono::high_resolution_clock::now();
        MonteCarloEngine::runSimulation(no_of_paths,spotPrice , strikePrice ,timeToMaturity, riskFreeRate , volatility);
        auto end_time = std::chrono::high_resolution_clock::now();

        int64_t time_in_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        times.push_back(time_in_microseconds);
    }   
    std::cout << "----------Vanilla Model Time stats---------- " << std::endl;
    std::cout << std::fixed << std::setprecision(6);
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

