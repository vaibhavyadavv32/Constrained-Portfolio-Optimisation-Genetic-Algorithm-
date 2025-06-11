#ifndef PORTFOLIO_HPP
#define PORTFOLIO_HPP

#include "Matrix.hpp"
#include <vector>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <limits>
#include <string>

/**
 * @brief Models a portfolio of assets with methods to compute returns, risk, and Sharpe ratio.
 */
class Portfolio
{
private:
    Matrix returns;               // T x N matrix of asset log-returns
    Matrix cov_matrix;           // N x N covariance matrix
    std::vector<double> means;   // Mean returns of assets
    size_t num_assets;           // Number of assets (N)

public:
    /**
     * @brief Constructs Portfolio from historical returns.
     * @param R Matrix of log-returns (rows = time, cols = assets).
     * @throws std::invalid_argument if fewer than 2 rows in R.
     */
    Portfolio(const Matrix& R);

    // --- Accessors ---

    size_t get_num_assets() const;
    const std::vector<double>& get_means() const;
    const Matrix& get_covariance() const;

    // --- Portfolio Metrics ---

    double portfolio_return(const std::vector<double>& weights) const;
    double excess_return(const std::vector<double>& weights, double risk_free) const;
    double portfolio_variance(const std::vector<double>& weights) const;
    double portfolio_risk(const std::vector<double>& weights) const;

    /**
     * @brief Computes Sharpe Ratio = (return - risk-free) / std dev.
     * Returns ∞ if risk is 0 and excess return > 0; 0 if excess ≤ 0.
     */
    double sharpe_ratio(const std::vector<double>& weights, double risk_free = 0.0) const;
};

#endif // PORTFOLIO_HPP
