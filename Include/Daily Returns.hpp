#pragma once

#include <vector>
#include <string>

// Optional: wrap in a namespace like `FinancialData` to avoid global scope pollution

class DataReader 
{
public:
    /**
     * @brief Reads price data from input file, computes log returns, and writes them to output file.
     * @param input_filepath Full path to CSV containing price data.
     * @param output_filepath Path to save the computed log returns.
     * @return True if processing is successful, false otherwise.
     */
    bool processAndSaveDailyReturns(const std::string& input_filepath, const std::string& output_filepath);

    // Accessors
    const std::vector<std::string>& getAssetTickers() const { return assetTickers; }
    const std::vector<std::vector<double>>& getDailyReturns() const { return dailyReturns; }

private:
    std::vector<std::string> assetTickers;                // Stock tickers (e.g., AAPL, MSFT)
    std::vector<std::vector<double>> prices;              // Raw historical prices
    std::vector<std::vector<double>> dailyReturns;        // Computed daily log returns
};
