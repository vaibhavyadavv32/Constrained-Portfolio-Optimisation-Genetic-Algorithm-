#ifndef CONSTRAINTS_HPP
#define CONSTRAINTS_HPP

#include <vector>
#include <stdexcept>
#include <numeric>
#include <cmath>
#include <limits>
#include <string>

const double EPSILON = 1e-8; // Tolerance for floating-point comparisons

// --- Constraint Checks ---

/**
 * @brief Ensures weights sum to 1.0 within tolerance.
 */
inline void validate_weights_sum_to_one(const std::vector<double>& weights) 
{
    double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    if (std::fabs(sum - 1.0) > EPSILON)
        throw std::invalid_argument("Weights must sum to 1.0. Got sum = " + std::to_string(sum));
}

/**
 * @brief Ensures each weight is within [lower, upper] bounds.
 */
inline void validate_bounds(const std::vector<double>& weights, double lower = 0.0, double upper = 1.0) {
    for (double w : weights) 
    {
        if (w < lower - EPSILON || w > upper + EPSILON)
            throw std::invalid_argument("Weight " + std::to_string(w) +
                " violates bounds [" + std::to_string(lower) + ", " + std::to_string(upper) + "]");
    }
}

/**
 * @brief Ensures expected portfolio return ≥ target return.
 */
inline void validate_target_return(
    const std::vector<double>& weights,
    const std::vector<double>& mean_returns,
    double target_return
) 
{
    if (weights.size() != mean_returns.size())
        throw std::invalid_argument("Weight and return vectors must match in size.");

    double expected_return = 0.0;
    for (size_t i = 0; i < weights.size(); ++i)
        expected_return += weights[i] * mean_returns[i];

    if (expected_return + EPSILON < target_return)
        throw std::invalid_argument("Expected return (" + std::to_string(expected_return) +
            ") is below target (" + std::to_string(target_return) + ").");
}

/**
 * @brief Runs all weight validation checks.
 */
inline void validate_weights(
    const std::vector<double>& weights,
    const std::vector<double>& mean_returns,
    double lower = 0.0,
    double upper = 1.0,
    double target_return = std::numeric_limits<double>::lowest()
) 
{
    validate_weights_sum_to_one(weights);
    validate_bounds(weights, lower, upper);
    if (target_return > std::numeric_limits<double>::lowest() + EPSILON)
        validate_target_return(weights, mean_returns, target_return);
}

/**
 * @brief Returns true if weights satisfy all constraints.
 */
inline bool is_feasible(
    const std::vector<double>& weights,
    const std::vector<double>& mean_returns,
    double lower = 0.0,
    double upper = 1.0,
    double target_return = std::numeric_limits<double>::lowest()
) 
{
    try 
    {
        validate_weights(weights, mean_returns, lower, upper, target_return);
        return true;
    } 
    catch (const std::invalid_argument&) 
    {
        return false;
    } 
    catch (...) 
    {
        return false;
    }
}

// --- Penalty Evaluation ---

/**
 * @brief Computes a penalty for constraint violations.
 *        Useful in genetic algorithm fitness evaluation.
 */
inline double constraint_penalty(
    const std::vector<double>& weights,
    const std::vector<double>& mean_returns,
    double lower = 0.0,
    double upper = 1.0,
    double target_return = std::numeric_limits<double>::lowest()
) 
{
    double penalty = 0.0;

    // Sum deviation
    double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    penalty += std::fabs(sum - 1.0);

    // Bounds violation
    for (double w : weights) 
    {
        if (w < lower - EPSILON) penalty += (lower - w);
        if (w > upper + EPSILON) penalty += (w - upper);
    }

    // Target return shortfall
    if (target_return > std::numeric_limits<double>::lowest() + EPSILON) 
    {
        if (weights.size() != mean_returns.size()) 
        {
            penalty += 1000.0; // Structural mismatch
        } 
        else 
        {
            double expected_return = 0.0;
            for (size_t i = 0; i < weights.size(); ++i)
                expected_return += weights[i] * mean_returns[i];

            if (expected_return + EPSILON < target_return)
                penalty += (target_return - expected_return);
        }
    }

    return penalty;
}

#endif // CONSTRAINTS_HPP
