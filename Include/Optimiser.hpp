#ifndef OPTIMISER_HPP
#define OPTIMISER_HPP

#include <vector>
#include <random>
#include <limits>
#include "Portfolio.hpp"
#include "Constraints.hpp"

/**
 * @brief Optimises portfolio weights using a genetic algorithm.
 */
class GeneticOptimiser
{
private:
    Portfolio portfolio;

    // Genetic algorithm configuration
    size_t population_size;
    size_t generations;
    double mutation_rate;
    double crossover_rate;
    double lower_bound;
    double upper_bound;
    double target_return;
    double risk_free_rate_;

    // Constraint penalty weights
    double sum_penalty_multiplier;
    double bounds_penalty_multiplier;
    double target_return_penalty_multiplier;

    std::mt19937 generator;

    // Fitness function: penalised negative Sharpe ratio
    double calculate_fitness(const std::vector<double>& weights) const;

    // Tournament selection (k=3)
    std::vector<double> select_parent(
        const std::vector<std::vector<double>>& population,
        const std::vector<double>& fitnesses);

    // Uniform crossover with re-normalization
    void crossover(
        const std::vector<double>& parent1,
        const std::vector<double>& parent2,
        std::vector<double>& child1,
        std::vector<double>& child2);

    // Mutation via Gaussian perturbation
    void mutate(std::vector<double>& weights);

public:
    GeneticOptimiser(
        const Portfolio& port,
        size_t population_size = 100,
        size_t generations = 1000,
        double mutation_rate = 0.05,
        double crossover_rate = 0.7,
        double lower = 0.0,
        double upper = 1.0,
        double target_return = std::numeric_limits<double>::lowest(),
        double sum_pen_mult = 100.0,
        double bounds_pen_mult = 100.0,
        double ret_pen_mult = 1000.0,
        double risk_free_rate = 0.0
    );

    // Run the genetic algorithm and return best weights
    std::vector<double> optimise();
};

#endif // OPTIMISER_HPP
