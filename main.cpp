#include <iostream>
#include <curl/curl.h>
#include <string>
#include <nlohmann/json.hpp> // For JSON parsing
#include "monte_carlo.h"

using json = nlohmann::json;

// Helper for libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    s->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Fetch stock data from Yahoo Finance
double fetch_stock_price(const std::string& symbol) {
    CURL* curl = curl_easy_init();
    std::string readBuffer;

    if(curl) {
        std::string url = "https://query1.finance.yahoo.com/v7/finance/quote?symbols=" + symbol;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        auto j = json::parse(readBuffer);
        return j["quoteResponse"]["result"][0]["regularMarketPrice"];
    }
    return 0.0;
}

int main() {
    std::string symbol = "AAPL";
    double S0 = fetch_stock_price(symbol);
    double mu = 0.1;       // Expected annual return
    double sigma = 0.2;    // Volatility
    double T = 1.0;        // 1 year
    int steps = 252;       // Trading days in a year
    int simulations = 10000;

    MonteCarlo mc(S0, mu, sigma, T, steps, simulations);
    auto results = mc.run();

    double sum = 0;
    for (auto price : results) sum += price;
    std::cout << "Expected stock price after 1 year: " << sum / simulations << std::endl;

    return 0;
}
