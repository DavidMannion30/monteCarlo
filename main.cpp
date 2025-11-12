#include <iostream>
#include <vector>
#include <cmath>
#include <curl/curl.h>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>
#include "monte_carlo.h"

using json = nlohmann::json;

// libcurl write callback
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    s->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Fetch historical stock data (daily close prices)
std::vector<double> fetch_historical_prices(const std::string& symbol, const std::string& interval="1d") {
    CURL* curl = curl_easy_init();
    std::string readBuffer;
    std::vector<double> prices;

    if(curl) {
        // Yahoo Finance URL for historical data (JSON)
        std::string url = "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol + "?interval=" + interval + "&range=1y";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        auto j = json::parse(readBuffer);
        auto timestamps = j["chart"]["result"][0]["timestamp"];
        auto close_prices = j["chart"]["result"][0]["indicators"]["quote"][0]["close"];

        for (auto &p : close_prices) {
            if (!p.is_null()) prices.push_back(p);
        }
    }
    return prices;
}

// Calculate daily returns
std::vector<double> calculate_daily_returns(const std::vector<double>& prices) {
    std::vector<double> returns;
    for (size_t i = 1; i < prices.size(); ++i) {
        returns.push_back((prices[i] / prices[i-1]) - 1.0);
    }
    return returns;
}

// Compute mean of a vector
double mean(const std::vector<double>& v) {
    double sum = 0;
    for (auto x : v) sum += x;
    return sum / v.size();
}

// Compute standard deviation of a vector
double stddev(const std::vector<double>& v, double mean_val) {
    double sum = 0;
    for (auto x : v) sum += (x - mean_val)*(x - mean_val);
    return sqrt(sum / (v.size() - 1));
}

int main() {
    std::string symbol = "AAPL";
    auto prices = fetch_historical_prices(symbol);

    if (prices.empty()) {
        std::cerr << "Failed to fetch historical prices.\n";
        return 1;
    }

    double S0 = prices.back(); // Last closing price

    auto daily_returns = calculate_daily_returns(prices);
    double daily_mean = mean(daily_returns);
    double daily_std = stddev(daily_returns, daily_mean);

    double mu = daily_mean * 252;         // Annualized return
    double sigma = daily_std * sqrt(252); // Annualized volatility

    std::cout << "Stock: " << symbol << "\n";
    std::cout << "Current Price: " << S0 << "\n";
    std::cout << "Annualized Return (mu): " << mu << "\n";
    std::cout << "Annualized Volatility (sigma): " << sigma << "\n";

    // Monte Carlo simulation
    double T = 1.0;        // 1 year
    int steps = 252;       // Trading days
    int simulations = 10000;

    MonteCarlo mc(S0, mu, sigma, T, steps, simulations);
    auto results = mc.run();

    double sum = 0;
    for (auto price : results) sum += price;

    std::cout << "Expected stock price after 1 year: " << sum / simulations << "\n";

    return 0;
}
