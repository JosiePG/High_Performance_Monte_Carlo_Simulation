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


std::pair<double, double> MonteCarloEngine::runSimulation(int no_of_paths, double spotPrice, 
    double strikePrice, double timeToMaturity, double riskFreeRate, double volatility) {
    
    std::vector<double> randomNumbers = generateRandomNormalVariables(no_of_paths);
    
    double sum = 0.0;
    double sum_squared = 0.0;
    double discount_factor = std::exp(-riskFreeRate * timeToMaturity);

    for (int i = 0; i < no_of_paths; i++) {
        // The geometric brownian motion formula to determine the underlying assets price
        double terminal_price = spotPrice * std::exp(
            (riskFreeRate - 0.5 * std::pow(volatility, 2.0)) * timeToMaturity + 
            volatility * std::sqrt(timeToMaturity) * randomNumbers[i]);
        
        double payoff = calculatePayOff(terminal_price, strikePrice);
        double discounted_payoff = discount_factor * payoff; // need to represent the underlying assets price in todays worth
        
        sum += discounted_payoff;
        sum_squared += discounted_payoff * discounted_payoff;
    }

    double estimated_value = sum / no_of_paths; // the average of all of the disocunted payoffs
    double variance = (sum_squared - no_of_paths * (estimated_value * estimated_value)) / (no_of_paths - 1);
    double standard_error = std::sqrt(variance / no_of_paths);

    return std::make_pair(estimated_value, standard_error);
}
