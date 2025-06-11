#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <locale>

// Custom headers
#include "Matrix.hpp"
#include "Portfolio.hpp"
#include "Optimiser.hpp"
#include "Utils.hpp"
#include "Daily Returns.hpp"

const double TRADING_DAYS_PER_YEAR = 252.0; // Trading days for annualization

// Load returns data from CSV into Matrix (skips date column)
Matrix load_returns_csv(const std::string& filename) 
{
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("File open failed: " + filename);

    std::vector<std::vector<double>> data;
    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) 
    {
        std::stringstream ss(line);
        std::vector<double> row;
        std::string cell;
        
        std::getline(ss, cell, ','); // Skip date column
        
        while (std::getline(ss, cell, ',')) 
        {
            try 
            {
                // Clean and convert cell to double
                cell.erase(0, cell.find_first_not_of(" \t\r\n"));
                cell.erase(cell.find_last_not_of(" \t\r\n") + 1);
                row.push_back(std::stod(cell));
            } 
            catch (...) 
            {
                row.push_back(0.0); // Default to 0 on conversion error
            }
        }
        
        if (!row.empty() && (data.empty() || row.size() == data[0].size())) 
        {
            data.push_back(row);
        }
    }

    if (data.empty()) throw std::runtime_error("No valid data in: " + filename);
    return Matrix(data);
}

// Save optimized weights to file
void save_weights_to_file(const std::vector<double>& weights, const std::string& filename) 
{
    std::ofstream outfile(filename);
    if (!outfile.is_open()) 
    {
        std::cerr << "Failed to open: " << filename << std::endl;
        return;
    }
    
    outfile << std::fixed << std::setprecision(8);
    for (double w : weights) outfile << w << "\n";
    outfile.close();
    std::cout << "Weights saved to: " << filename << std::endl;
}

// Save efficient frontier data (risk/return pairs)
void save_efficient_frontier_data(const std::vector<std::pair<double, double>>& frontier_data,
                                const std::string& filename) 
{
    std::ofstream outfile(filename);
    if (!outfile.is_open()) 
    {
        std::cerr << "Failed to open: " << filename << std::endl;
        return;
    }

    outfile << "Risk,Return\n" << std::fixed << std::setprecision(8);
    for (const auto& point : frontier_data) 
    {
        outfile << point.first << "," << point.second << "\n";
    }
    outfile.close();
    std::cout << "Frontier data saved to: " << filename << std::endl;
}

// Save portfolio metrics and weights
void save_optimised_portfolio_details(const std::vector<double>& weights,
                                    const std::string& filename,
                                    double annual_ret,
                                    double annual_risk,
                                    double annual_sharpe) 
{
    std::ofstream file(filename);
    if (!file.is_open()) 
    {
        std::cerr << "Failed to open: " << filename << std::endl;
        return;
    }

    file << "Metric,Value\n"
         << "Expected Return," << annual_ret << "\n"
         << "Portfolio Std Dev," << annual_risk << "\n"
         << "Sharpe Ratio," << annual_sharpe << "\n"
         << "Optimal Weights,";
    
    for (size_t i = 0; i < weights.size(); ++i) 
    {
        file << weights[i] << (i == weights.size() - 1 ? "" : ";");
    }
    file << "\n";
    file.close();
    std::cout << "Portfolio details saved to: " << filename << std::endl;
}

int main() 
{
    // Directory setup
    std::filesystem::path data_dir = "Data";
    std::filesystem::path results_dir = "Results";
    
    try 
    {
        std::filesystem::create_directories(results_dir);
    } 
    catch (...) 
    {
        std::cerr << "Failed to create results directory" << std::endl;
        return 1;
    }

    try 
    {
        // Data preparation
        std::string stocks_path = (data_dir / "stocks.csv").string();
        std::string returns_path = (data_dir / "Daily Returns.csv").string();

        // Generate returns file if needed
        if (!std::filesystem::exists(returns_path)) 
        {
            DataReader dr;
            if (!dr.processAndSaveDailyReturns(stocks_path, returns_path)) 
            {
                throw std::runtime_error("Failed to generate returns data");
            }
        }

        // Load returns data
        Matrix returns = load_returns_csv(returns_path);
        Portfolio portfolio(returns);

        // GA configuration
        const size_t POP_SIZE = 500;
        const size_t GENERATIONS = 1500;
        const double MUTATION_RATE = 0.05;
        const double CROSSOVER_RATE = 0.7;
        const double RISK_FREE_RATE = 0.02 / TRADING_DAYS_PER_YEAR; // Daily rate

        // Get user target return
        double target_annual;
        std::cout << "Enter target annual return (e.g., 0.10 for 10%): ";
        while (!(std::cin >> target_annual) || target_annual < -1.0) 
        {
            std::cout << "Invalid input. Enter a value (e.g., 0.05): ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        // Optimize for user target
        GeneticOptimiser user_optimiser(
            portfolio, POP_SIZE, GENERATIONS, MUTATION_RATE, CROSSOVER_RATE,
            0.0, 1.0, target_annual/TRADING_DAYS_PER_YEAR,
            1000.0, 1000.0, 5000.0, RISK_FREE_RATE
        );

        auto user_weights = user_optimiser.optimise();
        double user_ret = portfolio.portfolio_return(user_weights) * TRADING_DAYS_PER_YEAR;
        double user_risk = portfolio.portfolio_risk(user_weights) * sqrt(TRADING_DAYS_PER_YEAR);
        double user_sharpe = portfolio.sharpe_ratio(user_weights, RISK_FREE_RATE) * sqrt(TRADING_DAYS_PER_YEAR);

        // Save user results
        save_weights_to_file(user_weights, (results_dir / "User Weights.txt").string());
        save_optimised_portfolio_details(
            user_weights, (results_dir / "User Portfolio.csv").string(),
            user_ret, user_risk, user_sharpe
        );

        // Generate efficient frontier
        std::vector<std::pair<double, double>> frontier;
        const int FRONTIER_POINTS = 20;
        double min_ret = 0.0;
        double max_ret = 0.005 * TRADING_DAYS_PER_YEAR;
        double step = (max_ret - min_ret) / (FRONTIER_POINTS - 1);

        std::vector<double> best_sharpe_weights;
        double best_sharpe = -std::numeric_limits<double>::infinity();

        for (int i = 0; i < FRONTIER_POINTS; ++i) 
        {
            double target = (i == 0) ? std::numeric_limits<double>::lowest() 
                                   : min_ret + i * step / TRADING_DAYS_PER_YEAR;

            GeneticOptimiser ef_optimiser(
                portfolio, POP_SIZE, GENERATIONS, MUTATION_RATE, CROSSOVER_RATE,
                0.0, 1.0, target, 1000.0, 1000.0, 25000.0, RISK_FREE_RATE
            );

            auto weights = ef_optimiser.optimise();
            double ret = portfolio.portfolio_return(weights) * TRADING_DAYS_PER_YEAR;
            double risk = portfolio.portfolio_risk(weights) * sqrt(TRADING_DAYS_PER_YEAR);
            double sharpe = portfolio.sharpe_ratio(weights, RISK_FREE_RATE) * sqrt(TRADING_DAYS_PER_YEAR);

            frontier.push_back({risk, ret});
            if (sharpe > best_sharpe) 
            {
                best_sharpe = sharpe;
                best_sharpe_weights = weights;
            }
        }

        // Save frontier results
        std::sort(frontier.begin(), frontier.end());
        save_efficient_frontier_data(frontier, (results_dir / "Efficient Frontier.csv").string());
        
        if (!best_sharpe_weights.empty()) 
        {
            save_weights_to_file(best_sharpe_weights, (results_dir / "Best Sharpe Weights.txt").string());
            save_optimised_portfolio_details(
                best_sharpe_weights, (results_dir / "Best Sharpe Portfolio.csv").string(),
                portfolio.portfolio_return(best_sharpe_weights) * TRADING_DAYS_PER_YEAR,
                portfolio.portfolio_risk(best_sharpe_weights) * sqrt(TRADING_DAYS_PER_YEAR),
                best_sharpe
            );
        }
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Optimization complete" << std::endl;
    return 0;
}