#include <iostream>

int main() {
    double spotPrice;
    double strikePrice;
    double timeToMaturity; // Measured in years
    double riskFreeRate;
    double volatility;

    std::cout << "Monte Carlo Simulation For Option Pricing!" << std::endl;

    std::cout << "Enter spot price : "<< std::endl;
    std::cin >> spotPrice;
    std::cout << "Enter strike price : "<< std::endl;
    std::cin >> strikePrice;
    std::cout << "Enter time to maturity : "<< std::endl;
    std::cin >> timeToMaturity;
    std::cout << "Enter risk free rate: "<< std::endl;
    std::cin >> riskFreeRate;
    std::cout << "Enter volatility : "<< std::endl;
    std::cin >> volatility;

}