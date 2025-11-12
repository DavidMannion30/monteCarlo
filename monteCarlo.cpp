#include "monte_carlo.h"
#include <random>
#include <cmath>

MonteCarlo::MonteCarlo(double S0, double mu, double sigma, double T, int steps, int simulations)
    : S0(S0), mu(mu), sigma(sigma), T(T), steps(steps), simulations(simulations) {}

std::vector<double> MonteCarlo::run() {
    std::vector<double> results;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(0, 1);

    double dt = T / steps;

    for (int i = 0; i < simulations; ++i) {
        double S = S0;
        for (int j = 0; j < steps; ++j) {
            S *= exp((mu - 0.5 * sigma * sigma) * dt + sigma * sqrt(dt) * d(gen));
        }
        results.push_back(S);
    }

    return results;
}
