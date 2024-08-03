#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <random>
#include <cmath>
#include <fstream>

using namespace std::chrono;

// Структура для представления графа
struct Graph {
    int V, E;
    std::vector<std::vector<int>> adj;

    Graph(int V) : V(V), E(0) {
        adj.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        E++;
    }
};

std::unordered_set<int> greedyVertexCover(Graph& graph) {
    std::unordered_set<int> cover;
    std::vector<bool> coveredEdges(graph.E, false);
    std::vector<std::pair<int, int>> edges;

    for (int u = 0; u < graph.V; ++u) {
        for (int v : graph.adj[u]) {
            if (u < v) {
                edges.emplace_back(u, v);
            }
        }
    }

    while (std::count(coveredEdges.begin(), coveredEdges.end(), false) > 0) {
        int maxDegreeVertex = -1;
        int maxDegree = -1;

        for (int i = 0; i < graph.V; ++i) {
            if (cover.count(i)) continue;
            int degree = 0;
            for (int neighbor : graph.adj[i]) {
                if (!cover.count(neighbor)) {
                    degree++;
                }
            }

            if (degree > maxDegree) {
                maxDegree = degree;
                maxDegreeVertex = i;
            }
        }

        cover.insert(maxDegreeVertex);

        for (int i = 0; i < edges.size(); ++i) {
            if (!coveredEdges[i] && (edges[i].first == maxDegreeVertex || edges[i].second == maxDegreeVertex)) {
                coveredEdges[i] = true;
            }
        }
    }

    return cover;
}

void generateRandomGraph(Graph &graph, int E) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, graph.V - 1);

    for (int i = 0; i < E; ++i) {
        int u = dis(gen);
        int v = dis(gen);
        if (u != v) {
            graph.addEdge(u, v);
        } else {
            --i; // Не добавлять петли
        }
    }
}

void generateGridGraph(Graph &graph, int rows, int cols) {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int v = r * cols + c;
            if (c < cols - 1) graph.addEdge(v, v + 1);
            if (r < rows - 1) graph.addEdge(v, v + cols);
        }
    }
}

void generateClusterGraph(Graph &graph, int clusters, int clusterSize, int interClusterEdges) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> clusterDis(0, clusters - 1);

    int offset = 0;
    for (int i = 0; i < clusters; ++i) {
        for (int u = 0; u < clusterSize; ++u) {
            for (int v = u + 1; v < clusterSize; ++v) {
                graph.addEdge(offset + u, offset + v);
            }
        }
        offset += clusterSize;
    }

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
    std::unordered_set<int> result = greedyVertexCover(graph);
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(end - start).count();
    int coverSize = result.size();

    outputFile << V << "," << E << "," << graphType << "," << coverSize << "," << duration << "\n";

    std::cout << "Vertices: " << V << ", Edges: " << E << ", GraphType: " << graphType << ", CoverSize: " << coverSize << ", ExecutionTime: " << duration << " us" << std::endl;
}

int main() {
    std::ofstream outputFile("greedy_performance_data.csv");
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
