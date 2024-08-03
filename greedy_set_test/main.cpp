#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <random>
#include <cmath>
#include <fstream>

using namespace std::chrono;

// Функция для генерации случайных множеств
void generateRandomSets(int n, int m, std::vector<std::set<int>>& sets) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, n - 1);

    // Генерация случайных множеств
    for (int i = 0; i < m; ++i) {
        int setSize = dis(gen) % n + 1;
        while (sets[i].size() < setSize) {
            sets[i].insert(dis(gen));
        }
    }

    // Обеспечение покрытия всех элементов
    for (int i = 0; i < n; ++i) {
        int setIndex = dis(gen) % m;
        sets[setIndex].insert(i);
    }
}

// Функция для генерации множеств в виде сетки
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

    // Обеспечение покрытия всех элементов
    for (int i = 0; i < n; ++i) {
        int setIndex = rand() % m;
        sets[setIndex].insert(i);
    }
}

// Функция для генерации кластерных множеств
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

    // Обеспечение покрытия всех элементов
    for (int i = 0; i < n; ++i) {
        int setIndex = rand() % m;
        sets[setIndex].insert(i);
    }
}

void measurePerformance(int n, int m, int setType, std::ofstream &outputFile) {
    std::vector<std::set<int>> sets(m);

    if (setType == 1) {
        generateRandomSets(n, m, sets);
    } else if (setType == 2) {
        generateGridSets(n, m, sets);
    } else if (setType == 3) {
        generateClusterSets(n, m, sets, sqrt(n));
    }

    auto start = high_resolution_clock::now();

    std::unordered_set<int> covered;
    std::vector<int> cover;

    while (covered.size() < n) {
        int bestSet = -1;
        int maxCover = 0;
        for (int i = 0; i < m; ++i) {
            int currentCover = 0;
            for (int element : sets[i]) {
                if (covered.find(element) == covered.end()) {
                    currentCover++;
                }
            }
            if (currentCover > maxCover) {
                maxCover = currentCover;
                bestSet = i;
            }
        }

        if (bestSet == -1) {
            std::cout << "Cannot cover all elements." << std::endl;
            outputFile << n << "," << m << "," << setType << "," << 0 << "," << -1 << "\n";
            return;
        }

        cover.push_back(bestSet);
        for (int element : sets[bestSet]) {
            covered.insert(element);
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    std::cout << "Selected sets to cover all elements: ";
    for (int setIndex : cover) {
        std::cout << setIndex + 1 << " ";
    }
    std::cout << std::endl;
    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;

    outputFile << n << "," << m << "," << setType << "," << duration.count() << "," << cover.size() << "\n";
}

int main() {
    std::ofstream outputFile("performance_data_greedy.csv");
    outputFile << "Elements,Sets,SetType,ExecutionTime,CoverSize\n";

    std::cout << "Small sets (10 elements, 5 sets):" << std::endl;
    measurePerformance(10, 5, 1, outputFile);
    measurePerformance(10, 5, 2, outputFile);
    measurePerformance(10, 5, 3, outputFile);
    std::cout << "-----------------------------" << std::endl;

    std::cout << "Medium sets (100 elements, 50 sets):" << std::endl;
    measurePerformance(100, 50, 1, outputFile);
    measurePerformance(100, 50, 2, outputFile);
    measurePerformance(100, 50, 3, outputFile);
    std::cout << "-----------------------------" << std::endl;

    std::cout << "Large sets (1000 elements, 500 sets):" << std::endl;
    measurePerformance(1000, 500, 1, outputFile);
    measurePerformance(1000, 500, 2, outputFile);
    measurePerformance(1000, 500, 3, outputFile);
    std::cout << "-----------------------------" << std::endl;

    outputFile.close();
    return 0;
}
