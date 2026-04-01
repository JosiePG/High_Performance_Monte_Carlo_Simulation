#ifndef OPTION_PARAMETERS_H
#define OPTION_PARAMETERS_H

// All the inputs needed to price a European call option
struct OptionParameters {
    double spotPrice;       // current price of the asset (S)
    double strikePrice;     // price to buy the asset at expiry (K)
    double timeToMaturity;  // time until expiry in years (T)
    double riskFreeRate;    // risk-free interest rate (r)
    double volatility;      // annualised volatility of the asset (sigma)
};

#endif