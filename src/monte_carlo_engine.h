


#ifndef FINAL_YEAR_PROJECT_MONTE_CARLO_SIMULATION_H
#define FINAL_YEAR_PROJECT_MONTE_CARLO_SIMULATION_H
#include <vector>

class MonteCarloEngine {
protected:
    virtual std::vector<double> generateRandomNormalVariables(int no_of_paths);

    double calculatePayOff(double terminal_price,double strike_price);
public:

    virtual std::pair<double, double> runSimulation(int no_of_paths,double spotPrice , double strikePrice , double timeToMaturity,double riskFreeRate , double volatility);
};


#endif //FINAL_YEAR_PROJECT_MONTE_CARLO_SIMULATION_H