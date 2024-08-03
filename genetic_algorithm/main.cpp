#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std::chrono;

// Структура для представления индивидуума в популяции
struct Individual {
    std::vector<bool> chromosome;
    int fitness;
};

// Инициализация популяции случайными значениями
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

// Оценка пригодности (fitness) индивидуума
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

// Оценка пригодности для всей популяции
void evaluatePopulation(std::vector<Individual> &population, const std::vector<std::set<int>> &sets, int n) {
    for (auto &individual : population) {
        individual.fitness = evaluateFitness(individual, sets, n);
    }
}

// Выбор родителя для кроссовера
Individual selectParent(const std::vector<Individual> &population) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, population.size() - 1);
    return population[dis(gen)];
}

// Кроссовер (скрещивание) двух родителей
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

// Мутация индивидуума
void mutate(Individual &individual) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, individual.chromosome.size() - 1);
    int mutationPoint = dis(gen);
    individual.chromosome[mutationPoint] = !individual.chromosome[mutationPoint];
}

// Основная функция генетического алгоритма
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

int main() {
    int n, m;
    std::cout << "Enter number of elements:";
    std::cin >> n;
    std::cout << "Enter number of sets:";
    std::cin >> m;

    std::vector<std::set<int>> sets(m);
    for (int i = 0; i < m; ++i) {
        int setSize;
        std::cout << "Enter size of set " << i + 1 << ": ";
        std::cin >> setSize;
        std::cout << "Enter elements of set " << i + 1 << ": ";
        for (int j = 0; j < setSize; ++j) {
            int element;
            std::cin >> element;
            sets[i].insert(element);
        }
    }

    int populationSize, generations;
    std::cout << "Enter population size:";
    std::cin >> populationSize;
    std::cout << "Enter number of generations:";
    std::cin >> generations;

    auto start = high_resolution_clock::now();
    Individual result = geneticAlgorithm(sets, n, populationSize, generations);
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(end - start);
    std::cout << "Selected sets to cover all elements: ";
    for (int i = 0; i < result.chromosome.size(); ++i) {
        if (result.chromosome[i]) {
            std::cout << i + 1 << " ";
        }
    }
    std::cout << std::endl;
    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;

    return 0;
}
