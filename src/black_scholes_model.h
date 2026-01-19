

#ifndef FINAL_YEAR_PROJECT_BLACK_SCHOLES_MODEL_H
#define FINAL_YEAR_PROJECT_BLACK_SCHOLES_MODEL_H

#include <iostream>
#include <cmath>
#include <numbers>
#include <iomanip>

class BlackScholes {
private:
    double S;  // Current stock price
    double K;  // Strike price
    double T;  // Time to expiration (in years)
    double r;  // Risk-free rate
    double sigma; // Volatility

    // Cumulative standard normal distribution function
    double normalCDF(double x) const {
        return 0.5 * (1.0 + erf(x / sqrt(2.0)));
    }

    // Standard normal probability density function
    double normalPDF(double x) const {
        return (1.0 / sqrt(2.0 * std::numbers::pi)) * exp(-0.5 * x * x);
    }

    // Calculate d1 parameter
    double calculateD1() const {
        return (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
    }

    // Calculate d2 parameter
    double calculateD2() const {
        return calculateD1() - sigma * sqrt(T);
    }

public:
    // Constructor
    BlackScholes(double stockPrice, double strikePrice, double timeToExpiry,
                 double riskFreeRate, double volatility)
        : S(stockPrice), K(strikePrice), T(timeToExpiry), r(riskFreeRate), sigma(volatility) {}

    // Calculate call option price
    double callPrice() const {
        if (T <= 0) return std::max(S - K, 0.0);

        double d1 = calculateD1();
        double d2 = calculateD2();

        return S * normalCDF(d1) - K * exp(-r * T) * normalCDF(d2);
    }
};

#endif //FINAL_YEAR_PROJECT_BLACK_SCHOLES_MODEL_H