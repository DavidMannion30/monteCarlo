#pragma once
#include <vector>

class MonteCarlo {
public:
    MonteCarlo(double S0, double mu, double sigma, double T, int steps, int simulations);
    std::vector<double> run();

private:
    double S0;       // Initial stock price
    double mu;       // Expected return
    double sigma;    // Volatility
    double T;        // Time in years
    int steps;       // Steps per simulation
    int simulations; // Number of simulations
};
