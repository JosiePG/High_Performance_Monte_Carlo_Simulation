#include <iostream>
#include <cmath>
#include <bits/stdc++.h>
#include <random>
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

double MonteCarloEngine::simulatePayOffs(int no_of_paths,double terminal_prices[],double strike_price) {
    double sum_of_pay_offs = 0.0;
    for (int i=0;i<no_of_paths;i++) {
        sum_of_pay_offs+= calculatePayOff(terminal_prices[i],strike_price);
    }

    return sum_of_pay_offs;
}

double MonteCarloEngine::runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility) {
    clock_t start,end;
    start = clock();
    double terminal_prices[no_of_paths];
    std::vector<double> randomNumbers = generateRandomNormalVariables(no_of_paths);

    for (int i = 0;i<no_of_paths;i++) {

        terminal_prices[i] = spotPrice * std::exp(
            (riskFreeRate - 0.5 * std::pow(volatility,2.0))
            * timeToMaturity + volatility*std::sqrt(timeToMaturity)*randomNumbers[i]);
     }

    double sum = simulatePayOffs(no_of_paths,terminal_prices,strikePrice);

    double estimated_value = std::exp(-riskFreeRate*timeToMaturity) * (1.0/no_of_paths) * sum;
    end = clock();
    double time_taken_in_secs = double(end - start) / double(CLOCKS_PER_SEC);
    double time_taken_cpu_ticks = double(end - start);
    std::cout << "----------Vanilla monte carlo engine time stats---------- " << std::endl;
    std::cout << "Time taken by program is : " << time_taken_in_secs << std::setprecision(10);
    std::cout << " sec " << std::endl;
    std::cout << "Time taken by program is : " << time_taken_cpu_ticks << std::setprecision(10);
    std::cout << " cpu clock ticks " << std::endl;


    // need to change to call functions and return the estimated value of option
    return estimated_value;
}

