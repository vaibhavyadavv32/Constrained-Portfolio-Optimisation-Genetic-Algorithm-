#include "Optimiser.hpp"
#include "Constraints.hpp"
#include "Utils.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <limits>
#include <cmath>

// Constructor: Initialize optimiser and RNG
GeneticOptimiser::GeneticOptimiser(
    const Portfolio& port,
    size_t pop_size,
    size_t gens,
    double mut_rate,
    double cross_rate,
    double lower,
    double upper,
    double target_ret,
    double sum_pen_mult,
    double bounds_pen_mult,
    double ret_pen_mult,
    double daily_risk_free_rate
) : portfolio(port),
    population_size(pop_size),
    generations(gens),
    mutation_rate(mut_rate),
    crossover_rate(cross_rate),
    lower_bound(lower),
    upper_bound(upper),
    target_return(target_ret),
    sum_penalty_multiplier(sum_pen_mult),
    bounds_penalty_multiplier(bounds_pen_mult),
    target_return_penalty_multiplier(ret_pen_mult),
    risk_free_rate_(daily_risk_free_rate)
{
    std::random_device rd;
    generator.seed(rd());
}

// Evaluate fitness using Sharpe ratio and constraint penalties
double GeneticOptimiser::calculate_fitness(const std::vector<double>& weights) const
{
    double sum_penalty = std::fabs(std::accumulate(weights.begin(), weights.end(), 0.0) - 1.0);
    double bounds_penalty = 0.0;
    double sharpe = portfolio.sharpe_ratio(weights, risk_free_rate_);

    for (double w : weights) 
    {
        if (w < lower_bound) bounds_penalty += (lower_bound - w);
        if (w > upper_bound) bounds_penalty += (w - upper_bound);
    }

    double target_ret_penalty = 0.0;
    if (target_return > std::numeric_limits<double>::lowest() + EPSILON) {
        if (weights.size() != portfolio.get_num_assets()) 
        {
            target_ret_penalty = 100.0;
        } 
        else 
        {
            double current_return = portfolio.portfolio_return(weights);
            if (current_return + EPSILON < target_return)
                target_ret_penalty = target_return - current_return;
        }
    }

    double total_penalty = sum_penalty * sum_penalty_multiplier +
                           bounds_penalty * bounds_penalty_multiplier +
                           target_ret_penalty * target_return_penalty_multiplier;

    if (sharpe == std::numeric_limits<double>::infinity()) return -std::numeric_limits<double>::infinity();
    if (std::isnan(sharpe)) return std::numeric_limits<double>::max();

    return -sharpe + total_penalty;
}

// Tournament selection (k=3) to choose parent
std::vector<double> GeneticOptimiser::select_parent(
    const std::vector<std::vector<double>>& population,
    const std::vector<double>& fitnesses)
{
    size_t k = 3;
    std::uniform_int_distribution<> dis(0, population.size() - 1);

    size_t best_idx = dis(generator);
    for (size_t i = 1; i < k; ++i) 
    {
        size_t current_idx = dis(generator);
        if (fitnesses[current_idx] < fitnesses[best_idx])
            best_idx = current_idx;
    }
    return population[best_idx];
}

// Uniform crossover between two parents
void GeneticOptimiser::crossover(
    const std::vector<double>& parent1,
    const std::vector<double>& parent2,
    std::vector<double>& child1,
    std::vector<double>& child2)
{
    child1 = parent1;
    child2 = parent2;
    std::bernoulli_distribution coin(crossover_rate);

    for (size_t i = 0; i < parent1.size(); ++i)
        if (coin(generator)) std::swap(child1[i], child2[i]);

    normalize(child1);
    normalize(child2);
}

// Add Gaussian noise to genes and re-normalize
void GeneticOptimiser::mutate(std::vector<double>& individual)
{
    std::uniform_real_distribution<> dis(0.0, 1.0);
    std::normal_distribution<> noise(0.0, 0.05);
    bool mutated = false;

    for (double& w : individual) 
    {
        if (dis(generator) < mutation_rate) 
        {
            w += noise(generator);
            mutated = true;
        }
    }

    if (mutated) 
    {
        clip_weights(individual, lower_bound, upper_bound);
        normalize(individual);
    }
}

// Genetic algorithm driver
std::vector<double> GeneticOptimiser::optimise()
{
    size_t num_assets = portfolio.get_num_assets();
    std::vector<std::vector<double>> population(population_size);
    for (auto& ind : population) 
    {
        ind = random_weights(num_assets, generator);
        clip_weights(ind, lower_bound, upper_bound);
        normalize(ind);
    }

    std::vector<double> fitnesses(population_size);
    std::vector<double> best_weights;
    double best_fitness = std::numeric_limits<double>::max();

    for (size_t gen = 0; gen < generations; ++gen)
    {
        for (size_t i = 0; i < population_size; ++i)
            fitnesses[i] = calculate_fitness(population[i]);

        size_t best_idx = std::min_element(fitnesses.begin(), fitnesses.end()) - fitnesses.begin();
        if (fitnesses[best_idx] < best_fitness) 
        {
            best_fitness = fitnesses[best_idx];
            best_weights = population[best_idx];
        }

        std::vector<std::pair<double, std::vector<double>>> sorted;
        for (size_t i = 0; i < population_size; ++i)
            sorted.emplace_back(fitnesses[i], population[i]);
        std::sort(sorted.begin(), sorted.end());

        std::vector<std::vector<double>> new_pop;
        size_t elite_count = population_size / 20;
        for (size_t i = 0; i < elite_count; ++i)
            new_pop.push_back(sorted[i].second);

        while (new_pop.size() < population_size) 
        {
            auto p1 = select_parent(population, fitnesses);
            auto p2 = select_parent(population, fitnesses);
            std::vector<double> c1, c2;
            crossover(p1, p2, c1, c2);
            mutate(c1);
            mutate(c2);
            new_pop.push_back(c1);
            if (new_pop.size() < population_size)
                new_pop.push_back(c2);
        }

        population = std::move(new_pop);

        size_t log_interval = std::max<size_t>(1, generations / 10);
        if ((gen + 1) % log_interval == 0 || gen == 0 || gen == generations - 1) 
        {
            std::cout << "Generation " << gen + 1 << "/" << generations
                    << " | Best Fitness: " << std::fixed << std::setprecision(6) << best_fitness
                    << " | Current Gen Best Fitness: " << std::fixed << std::setprecision(6) << fitnesses[best_idx]
                    << std::endl;
        }
    }

    double final_fitness = calculate_fitness(best_weights);
    if (final_fitness < best_fitness)
        best_fitness = final_fitness;

    std::cout << "\nOptimization complete. Best overall fitness: "
            << std::fixed << std::setprecision(6) << best_fitness << std::endl;

    return best_weights;
}
