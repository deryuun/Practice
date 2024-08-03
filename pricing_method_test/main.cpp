#include <iostream>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <random>
#include <algorithm>
#include <cmath>
#include <fstream>

using namespace std::chrono;

struct Graph {
    int V, E;
    std::vector<std::vector<int>> adj;
    std::vector<int> weights;

    Graph(int V) : V(V), E(0) {
        adj.resize(V);
        weights.resize(V, 0);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        E++;
    }

    void setWeight(int v, int w) {
        weights[v] = w;
    }
};

bool isTight(const std::vector<int>& prices, const Graph& graph, int v) {
    int sumPrices = 0;
    for (int u : graph.adj[v]) {
        sumPrices += prices[u];
    }
    return sumPrices >= graph.weights[v];
}

std::vector<int> pricingMethod(const Graph& graph) {
    int V = graph.V;
    std::vector<int> prices(V, 0);
    std::unordered_set<int> vertexCover;

    while (true) {
        bool found = false;

        for (int u = 0; u < V; ++u) {
            for (int v : graph.adj[u]) {
                if (!isTight(prices, graph, u) && !isTight(prices, graph, v)) {
                    int maxIncrease = std::min(graph.weights[u] - prices[u], graph.weights[v] - prices[v]);
                    prices[u] += maxIncrease;
                    prices[v] += maxIncrease;
                    found = true;
                }
            }
        }

        if (!found) break;

        for (int i = 0; i < V; ++i) {
            if (isTight(prices, graph, i)) {
                vertexCover.insert(i);
            }
        }
    }

    return std::vector<int>(vertexCover.begin(), vertexCover.end());
}

void generateRandomGraph(Graph &graph, int E) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, graph.V - 1);
    std::uniform_int_distribution<> weightDis(1, 10);

    for (int i = 0; i < E; ++i) {
        int u = dis(gen);
        int v = dis(gen);
        if (u != v) {
            graph.addEdge(u, v);
        } else {
            --i;
        }
    }

    for (int i = 0; i < graph.V; ++i) {
        graph.setWeight(i, weightDis(gen));
    }
}

void generateGridGraph(Graph &graph, int rows, int cols) {
    std::uniform_int_distribution<> weightDis(1, 10);
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int v = r * cols + c;
            if (c < cols - 1) graph.addEdge(v, v + 1);
            if (r < rows - 1) graph.addEdge(v, v + cols);
            graph.setWeight(v, weightDis(gen));
        }
    }
}

void generateClusterGraph(Graph &graph, int clusters, int clusterSize, int interClusterEdges) {
    std::uniform_int_distribution<> weightDis(1, 10);
    std::random_device rd;
    std::mt19937 gen(rd());

    int offset = 0;
    for (int i = 0; i < clusters; ++i) {
        for (int u = 0; u < clusterSize; ++u) {
            for (int v = u + 1; v < clusterSize; ++v) {
                graph.addEdge(offset + u, offset + v);
            }
            graph.setWeight(offset + u, weightDis(gen));
        }
        offset += clusterSize;
    }

    std::uniform_int_distribution<> clusterDis(0, clusters - 1);
    for (int i = 0; i < interClusterEdges; ++i) {
        int c1 = clusterDis(gen) * clusterSize + std::uniform_int_distribution<>(0, clusterSize - 1)(gen);
        int c2 = clusterDis(gen) * clusterSize + std::uniform_int_distribution<>(0, clusterSize - 1)(gen);
        if (c1 != c2) {
            graph.addEdge(c1, c2);
        } else {
            --i;
        }
    }
}

void measurePerformance(int V, int E, int graphType, std::ofstream &outputFile) {
    Graph graph(V);

    if (graphType == 1) {
        generateRandomGraph(graph, E);
    } else if (graphType == 2) {
        int rows = std::sqrt(V);
        int cols = V / rows;
        generateGridGraph(graph, rows, cols);
    } else if (graphType == 3) {
        int clusters = std::sqrt(V);
        int clusterSize = V / clusters;
        int interClusterEdges = E - (clusters * clusterSize * (clusterSize - 1) / 2);
        generateClusterGraph(graph, clusters, clusterSize, interClusterEdges);
    }

    auto start = high_resolution_clock::now();
    std::vector<int> result = pricingMethod(graph);
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(end - start).count();
    int coverSize = result.size();

    outputFile << V << "," << E << "," << graphType << "," << coverSize << "," << duration << "\n";

    std::cout << "Vertices: " << V << ", Edges: " << E << ", GraphType: " << graphType << ", CoverSize: " << coverSize << ", ExecutionTime: " << duration << " us" << std::endl;
}

int main() {
    std::ofstream outputFile("performance_data.csv");
    outputFile << "Vertices,Edges,GraphType,CoverSize,ExecutionTime\n";

    std::cout << "Small graph (10 vertices, 15 edges):" << std::endl;
    measurePerformance(10, 15, 1, outputFile);
    measurePerformance(10, 15, 2, outputFile);
    measurePerformance(10, 15, 3, outputFile);
    std::cout << "-----------------------------" << std::endl;

    std::cout << "Medium graph (100 vertices, 500 edges):" << std::endl;
    measurePerformance(100, 500, 1, outputFile);
    measurePerformance(100, 500, 2, outputFile);
    measurePerformance(100, 500, 3, outputFile);
    std::cout << "-----------------------------" << std::endl;

    std::cout << "Large graph (1000 vertices, 5000 edges):" << std::endl;
    measurePerformance(1000, 5000, 1, outputFile);
    measurePerformance(1000, 5000, 2, outputFile);
    measurePerformance(1000, 5000, 3, outputFile);

    outputFile.close();

    return 0;
}
