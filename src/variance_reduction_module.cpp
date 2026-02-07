#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <algorithm>


#include "variance_reduction_module.h"
#include "black_scholes_model.h"

double VarianceReductionModule::blackScholesPrice(double spotPrice,double strikePrice,double timeToExpiration,double riskFreeRate,double volatility) {
    BlackScholes bs_model(spotPrice,strikePrice,timeToExpiration,riskFreeRate,volatility);
    return bs_model.callPrice();

}


std::pair<double,double> VarianceReductionModule::runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility){
    std::vector<double> randomNumbers = generateRandomNormalVariables(no_of_paths);

    double sum = 0.0;
    double sum_squared = 0.0;
    double discount_factor = std::exp(-riskFreeRate * timeToMaturity);

    for (int i = 0;i<no_of_paths;i++) {

        double terminal_price = spotPrice * std::exp(
            (riskFreeRate - 0.5 * std::pow(volatility,2.0))
            * timeToMaturity + volatility*std::sqrt(timeToMaturity)*randomNumbers[i]);
        double antithetic_terminal_price = spotPrice * std::exp(
        (riskFreeRate - 0.5 * std::pow(volatility,2.0))
        * timeToMaturity + volatility*std::sqrt(timeToMaturity)*-randomNumbers[i]);
    
        double payoff = calculatePayOff(terminal_price, strikePrice);
        double antithetic_payoff = calculatePayOff(antithetic_terminal_price, strikePrice);
        double discounted_payoff = discount_factor * payoff;
        double discounted_antithetic_payoff = discount_factor * antithetic_payoff;
        double average_payoff = 0.5 * (discounted_payoff +discounted_antithetic_payoff);
        sum += average_payoff;
        sum_squared += average_payoff * average_payoff;
    
    }



    double antithetic_estimated_value = sum/no_of_paths;
    double variance = (sum_squared - no_of_paths * (antithetic_estimated_value * antithetic_estimated_value)) / (no_of_paths - 1);
    double standard_error = std::sqrt(variance / no_of_paths);

    return std::make_pair(antithetic_estimated_value, standard_error);
}

