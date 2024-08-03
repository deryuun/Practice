#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>
#include <cmath>

using namespace std::chrono;

struct Individual {
    std::vector<bool> chromosome;
    int fitness;
};

std::vector<Individual> initializePopulation(int populationSize, int setSize) {
    std::vector<Individual> population(populationSize);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (int i = 0; i < populationSize; ++i) {
        population[i].chromosome.resize(setSize);
        for (int j = 0; j < setSize; ++j) {
            population[i].chromosome[j] = dis(gen);
        }
        population[i].fitness = 0;
    }
    return population;
}

int evaluateFitness(const Individual &individual, const std::vector<std::set<int>> &sets, int n) {
    std::unordered_set<int> covered;
    for (int i = 0; i < individual.chromosome.size(); ++i) {
        if (individual.chromosome[i]) {
            for (int elem : sets[i]) {
                covered.insert(elem);
            }
        }
    }
    return covered.size() == n ? covered.size() - std::count(individual.chromosome.begin(), individual.chromosome.end(), true) : -1;
}

void evaluatePopulation(std::vector<Individual> &population, const std::vector<std::set<int>> &sets, int n) {
    for (auto &individual : population) {
        individual.fitness = evaluateFitness(individual, sets, n);
    }
}

Individual selectParent(const std::vector<Individual> &population) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, population.size() - 1);
    return population[dis(gen)];
}

Individual crossover(const Individual &parent1, const Individual &parent2) {
    Individual offspring;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, parent1.chromosome.size() - 1);
    int crossoverPoint = dis(gen);

    offspring.chromosome.insert(offspring.chromosome.end(), parent1.chromosome.begin(), parent1.chromosome.begin() + crossoverPoint);
    offspring.chromosome.insert(offspring.chromosome.end(), parent2.chromosome.begin() + crossoverPoint, parent2.chromosome.end());
    return offspring;
}

void mutate(Individual &individual) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, individual.chromosome.size() - 1);
    int mutationPoint = dis(gen);
    individual.chromosome[mutationPoint] = !individual.chromosome[mutationPoint];
}

Individual geneticAlgorithm(const std::vector<std::set<int>> &sets, int n, int populationSize, int generations) {
    auto population = initializePopulation(populationSize, sets.size());
    evaluatePopulation(population, sets, n);

    for (int gen = 0; gen < generations; ++gen) {
        auto parent1 = selectParent(population);
        auto parent2 = selectParent(population);
        auto offspring = crossover(parent1, parent2);
        mutate(offspring);
        offspring.fitness = evaluateFitness(offspring, sets, n);

        auto worstIt = std::min_element(population.begin(), population.end(), [](const Individual &a, const Individual &b) {
            return a.fitness < b.fitness;
        });

        if (offspring.fitness > worstIt->fitness) {
            *worstIt = offspring;
        }
    }

    return *std::max_element(population.begin(), population.end(), [](const Individual &a, const Individual &b) {
        return a.fitness < b.fitness;
    });
}

void generateRandomSets(int n, int m, std::vector<std::set<int>>& sets) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, n - 1);

    for (int i = 0; i < m; ++i) {
        int setSize = dis(gen) % n + 1;
        while (sets[i].size() < setSize) {
            sets[i].insert(dis(gen));
        }
    }
}

void generateGridSets(int n, int m, std::vector<std::set<int>>& sets) {
    int size = sqrt(n);
    for (int i = 0; i < m; ++i) {
        int row = i / size;
        int col = i % size;
        if (row < size - 1) {
            sets[i].insert(row * size + col);
            sets[i].insert((row + 1) * size + col);
        }
        if (col < size - 1) {
            sets[i].insert(row * size + col);
            sets[i].insert(row * size + col + 1);
        }
    }
}

void generateClusterSets(int n, int m, std::vector<std::set<int>>& sets, int clusterSize) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, clusterSize - 1);

    int clusters = n / clusterSize;
    for (int i = 0; i < clusters; ++i) {
        for (int j = 0; j < m / clusters; ++j) {
            int setSize = dis(gen) % clusterSize + 1;
            while (sets[i * (m / clusters) + j].size() < setSize) {
                sets[i * (m / clusters) + j].insert(i * clusterSize + dis(gen));
            }
        }
    }

    for (int i = 0; i < m - clusters * (m / clusters); ++i) {
        int setSize = dis(gen) % clusterSize + 1;
        while (sets[clusters * (m / clusters) + i].size() < setSize) {
            sets[clusters * (m / clusters) + i].insert(dis(gen));
        }
    }
}

void measurePerformance(int n, int m, int setType, int populationSize, int generations, std::ofstream& outputFile) {
    std::vector<std::set<int>> sets(m);

    if (setType == 1) {
        generateRandomSets(n, m, sets);
    } else if (setType == 2) {
        generateGridSets(n, m, sets);
    } else if (setType == 3) {
        generateClusterSets(n, m, sets, sqrt(n));
    }

    auto start = high_resolution_clock::now();
    Individual result = geneticAlgorithm(sets, n, populationSize, generations);
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(end - start);

    outputFile << n << "," << m << "," << setType << "," << result.fitness << "," << duration.count() << std::endl;

    std::cout << "Selected sets to cover all elements: ";
    for (int i = 0; i < result.chromosome.size(); ++i) {
        if (result.chromosome[i]) {
            std::cout << i + 1 << " ";
        }
    }
    std::cout << std::endl;
    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;
}

int main() {
    std::ofstream outputFile("genetic_algorithm_performance.csv");
    outputFile << "Elements,Sets,SetType,Fitness,ExecutionTime" << std::endl;

    int populationSize = 50;
    int generations = 100;

    std::cout << "Small sets (10 elements, 5 sets):" << std::endl;
    measurePerformance(10, 5, 1, populationSize, generations, outputFile);
    measurePerformance(10, 5, 2, populationSize, generations, outputFile);
    measurePerformance(10, 5, 3, populationSize, generations, outputFile);
    std::cout << "-----------------------------" << std::endl;

    std::cout << "Medium sets (100 elements, 50 sets):" << std::endl;
    measurePerformance(100, 50, 1, populationSize, generations, outputFile);
    measurePerformance(100, 50, 2, populationSize, generations, outputFile);
    measurePerformance(100, 50, 3, populationSize, generations, outputFile);
    std::cout << "-----------------------------" << std::endl;

    std::cout << "Large sets (1000 elements, 500 sets):" << std::endl;
    measurePerformance(1000, 500, 1, populationSize, generations, outputFile);
    measurePerformance(1000, 500, 2, populationSize, generations, outputFile);
    measurePerformance(1000, 500, 3, populationSize, generations, outputFile);
    std::cout << "-----------------------------" << std::endl;

    outputFile.close();
    return 0;
}
