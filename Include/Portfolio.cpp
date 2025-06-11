#include "Portfolio.hpp" // Include the header file for Portfolio class
#include <algorithm>     // For std::max
#include <iostream>      // For debugging, remove later

// Implementations of Portfolio class methods

Portfolio::Portfolio(const Matrix& R) : returns(R)
{
    if (R.num_rows() < 2) 
    {
        throw std::invalid_argument("Portfolio constructor requires a returns matrix with at least 2 rows of data.");
    }

    num_assets = R.num_cols();
    if (num_assets == 0) 
    {
        throw std::invalid_argument("Portfolio constructor requires a returns matrix with at least 1 asset column.");
    }

    // Compute means and covariance matrix using Matrix class methods
    means = R.mean_per_column();
    cov_matrix = R.covariance_matrix();
}

size_t Portfolio::get_num_assets() const
{
    return num_assets;
}

const std::vector<double>& Portfolio::get_means() const 
{
    return means;
}

const Matrix& Portfolio::get_covariance() const {
    return cov_matrix;
}

double Portfolio::portfolio_return(const std::vector<double>& weights) const
{
    if (weights.size() != num_assets) 
    {
        throw std::invalid_argument("Weight vector size mismatch for portfolio return calculation. Expected " +
                                    std::to_string(num_assets) + ", got " + std::to_string(weights.size()) + ".");
    }

    double ret = 0.0;
    for (size_t i = 0; i < num_assets; ++i)
        ret += weights[i] * means[i];
    return ret;
}

double Portfolio::excess_return(const std::vector<double>& weights, double risk_free) const
{
    return portfolio_return(weights) - risk_free;
}

double Portfolio::portfolio_variance(const std::vector<double>& weights) const
{
    if (weights.size() != num_assets) 
    {
        throw std::invalid_argument("Weight vector size mismatch for portfolio variance calculation. Expected " +
                                    std::to_string(num_assets) + ", got " + std::to_string(weights.size()) + ".");
    }

    double var = 0.0;
    // This calculates w^T * Cov * w manually for clarity.
    // It could also be implemented using Matrix operations:
    // Matrix w_mat(weights.size(), 1); // convert weights to a column matrix
    // for(size_t i=0; i<weights.size(); ++i) w_mat(i,0) = weights[i];
    // return (w_mat.transpose().dot(cov_matrix.dot(w_mat)))(0,0);
    for (size_t i = 0; i < num_assets; ++i)
        for (size_t j = 0; j < num_assets; ++j)
            var += weights[i] * weights[j] * cov_matrix(i, j); // Element-wise product

    return var;
}

double Portfolio::portfolio_risk(const std::vector<double>& weights) const
{
    double var = portfolio_variance(weights);
    // Ensure variance is non-negative before taking square root, due to floating point arithmetic
    return std::sqrt(std::max(0.0, var));
}

double Portfolio::sharpe_ratio(const std::vector<double>& weights, double risk_free) const
{
    double port_ret = portfolio_return(weights);
    double port_risk = portfolio_risk(weights);
    double excess_ret = port_ret - risk_free;

    if (port_risk < 1e-12) 
    { // Use a small epsilon to check for near-zero risk
        if (excess_ret > 1e-12) 
        { // If risk is zero and excess return is positive
            return std::numeric_limits<double>::infinity();
        } 
        else 
        { // If risk is zero and excess return is zero or negative
            return 0.0; // Or -infinity, depending on desired behavior. 0.0 is safer for optimization.
        }
    }
    return excess_ret / port_risk;
}