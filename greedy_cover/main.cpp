#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>

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

    // Создаем список ребер
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

        // Находим вершину с наибольшей степенью (количеством непокрытых соседей)
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

        // Добавляем вершину с наибольшей степенью в покрытие
        cover.insert(maxDegreeVertex);

        // Отмечаем все инцидентные ребра как покрытые
        for (int i = 0; i < edges.size(); ++i) {
            if (!coveredEdges[i] && (edges[i].first == maxDegreeVertex || edges[i].second == maxDegreeVertex)) {
                coveredEdges[i] = true;
            }
        }
    }

    return cover;
}

int main() {
    int V;
    int E;
    std::cout << "Enter number of vertices:";
    std::cin >> V;
    std::cout << "Enter number of edges:";
    std::cin >> E;

    Graph graph(V);

    std::cout << "\nEnter the edges (u v) format:" << std::endl;
    for (int i = 0; i < E; ++i) {
        int u, v;
        std::cin >> u >> v;
        graph.addEdge(u, v);
    }

    std::unordered_set<int> cover = greedyVertexCover(graph);

    std::cout << "Vertex Cover: ";
    for (int v : cover) {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    return 0;
}
