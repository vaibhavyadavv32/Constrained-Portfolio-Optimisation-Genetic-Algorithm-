#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <random>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <cmath>

// --- Random Weight Generation ---

/**
 * @brief Generates random weights summing to 1.
 * @throws std::invalid_argument if n == 0.
 */
inline std::vector<double> random_weights(size_t n)
{
    if (n == 0) throw std::invalid_argument("Weight vector size must be > 0.");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    std::vector<double> weights(n);
    for (double& w : weights) w = dis(gen);

    double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    if (sum < 1e-12) {
        std::fill(weights.begin(), weights.end(), 1.0 / n);
    } else {
        for (double& w : weights) w /= sum;
    }
    return weights;
}

/**
 * @brief Random weight generator using external RNG.
 * @throws std::invalid_argument if n == 0.
 */
inline std::vector<double> random_weights(size_t n, std::mt19937& gen)
{
    if (n == 0) throw std::invalid_argument("Weight vector size must be > 0.");

    std::uniform_real_distribution<> dis(0.0, 1.0);
    std::vector<double> weights(n);
    for (double& w : weights) w = dis(gen);

    double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    if (sum < 1e-12) {
        std::fill(weights.begin(), weights.end(), 1.0 / n);
    } else {
        for (double& w : weights) w /= sum;
    }
    return weights;
}

// --- Vector Operations ---

/**
 * @brief Computes dot product of two vectors.
 * @throws std::invalid_argument if sizes mismatch.
 */
inline double dot_product(const std::vector<double>& a, const std::vector<double>& b)
{
    if (a.size() != b.size()) throw std::invalid_argument("Vector sizes must match.");
    double result = 0.0;
    for (size_t i = 0; i < a.size(); ++i)
        result += a[i] * b[i];
    return result;
}

// --- Weight Modifiers ---

/**
 * @brief Clips weights to [min_val, max_val].
 */
inline void clip_weights(std::vector<double>& weights, double min_val = 0.0, double max_val = 1.0)
{
    for (double& w : weights)
        w = std::clamp(w, min_val, max_val);
}

/**
 * @brief Normalizes weights to sum to 1.
 * @throws std::invalid_argument if sum ≈ 0.
 */
inline void normalize(std::vector<double>& weights)
{
    double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    if (sum < 1e-12)
        throw std::invalid_argument("Cannot normalize: sum ≈ 0.");
    for (double& w : weights) w /= sum;
}

// --- Precision Helper ---

/**
 * @brief Checks if two doubles are approximately equal.
 */
inline bool almost_equal(double a, double b, double eps = 1e-8)
{
    return std::fabs(a - b) < eps;
}

#endif // UTILS_HPP