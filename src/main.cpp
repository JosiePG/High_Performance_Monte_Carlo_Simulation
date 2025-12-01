#include <iostream>
#include "monte_carlo_engine.h"
#include "variance_reduction_module.h"
int main() {
    double spotPrice;
    double strikePrice;
    double timeToMaturity; // Measured in years
    double riskFreeRate;
    double volatility;
    int no_of_paths;

    std::cout << "Monte Carlo Simulation For Option Pricing!" << std::endl;

    // std::cout << "Enter spot price : "<< std::endl;
    // std::cin >> spotPrice;
    // std::cout << "Enter strike price : "<< std::endl;
    // std::cin >> strikePrice;
    // std::cout << "Enter time to maturity : "<< std::endl;
    // std::cin >> timeToMaturity;
    // std::cout << "Enter risk free rate: "<< std::endl;
    // std::cin >> riskFreeRate;
    // std::cout << "Enter volatility : "<< std::endl;
    // std::cin >> volatility;
    // std::cout << "Enter number of simulations : "<< std::endl;
    // std::cin >> no_of_paths;

    MonteCarloEngine vanilla_model;

    VarianceReductionModule variance_reduction_model;

    //double vanilla_estimated_value = vanilla_model.runSimulation(no_of_paths,spotPrice,strikePrice,timeToMaturity,riskFreeRate,volatility);
    //double variance_reduction_estimated_value = variance_reduction_model.runSimulation(no_of_paths,spotPrice,strikePrice,timeToMaturity,riskFreeRate,volatility);

    double vanilla_estimated_value = vanilla_model.runSimulation(10000,100.0,100.0,1.0,0.05,0.2);
    double variance_reduction_estimated_value = variance_reduction_model.runSimulation(10000,100.0,100.0,1.0,0.05,0.2);
    std::cout << "This is the vanilla model estimated value of the option: " << vanilla_estimated_value << std::endl;
    std::cout << "This is the variance reduction model estimated value of the option: " << variance_reduction_estimated_value << std::endl;


}