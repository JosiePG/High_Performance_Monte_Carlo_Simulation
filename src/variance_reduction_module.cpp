#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>

#include "variance_reduction_module.h"
#include "black_scholes_model.h"

double VarianceReductionModule::blackScholesPrice(double spotPrice,double strikePrice,double timeToExpiration,double riskFreeRate,double volatility) {
    BlackScholes bs_model(spotPrice,strikePrice,timeToExpiration,riskFreeRate,volatility);
    return bs_model.callPrice();

}


double VarianceReductionModule::runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility){
    clock_t start,end;
    start = clock();
    std::vector<double> terminal_prices(no_of_paths);
    std::vector<double> antithetic_terminal_prices(no_of_paths);
    std::vector<double> randomNumbers = generateRandomNormalVariables(no_of_paths);

    for (int i = 0;i<no_of_paths;i++) {

        terminal_prices[i] = spotPrice * std::exp(
            (riskFreeRate - 0.5 * std::pow(volatility,2.0))
            * timeToMaturity + volatility*std::sqrt(timeToMaturity)*randomNumbers[i]);
        antithetic_terminal_prices[i] = spotPrice * std::exp(
        (riskFreeRate - 0.5 * std::pow(volatility,2.0))
        * timeToMaturity + volatility*std::sqrt(timeToMaturity)*-randomNumbers[i]);
    }

    double sum = 0.5 * (simulatePayOffs(no_of_paths,terminal_prices,strikePrice)+simulatePayOffs(no_of_paths,antithetic_terminal_prices,strikePrice));

    double antithetic_estimated_value = std::exp(-riskFreeRate*timeToMaturity) * (1.0/no_of_paths) * sum;

    double bs_price = blackScholesPrice(spotPrice,strikePrice,timeToMaturity,riskFreeRate,volatility);

    double estimated_value = antithetic_estimated_value + (bs_price-antithetic_estimated_value);
    end = clock();
    double time_taken_in_secs = double(end - start) / double(CLOCKS_PER_SEC);
    double time_taken_cpu_ticks = double(end - start);
    std::cout << "----------Variance reduction module time stats---------- " << std::endl;
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Time taken by program is : " << time_taken_in_secs ;
    std::cout << " sec " << std::endl;
    std::cout << "Time taken by program is : " << time_taken_cpu_ticks ;
    std::cout << " cpu clock ticks " << std::endl;



    // need to change to call functions and return the estimated value of option
    return estimated_value;
}
