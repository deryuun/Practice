#include <iostream>
#include <vector>
#include <unordered_set>


// Структура для представления графа
struct Graph {
    int V;
    int E;
    std::vector<std::vector<int>> adj;
    std::vector<int> weights;

    Graph(int V, int E) : V(V), E(E) {
        adj.resize(V);
        weights.resize(V, 0);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
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
    std::vector<int> prices(V, 0); // начальные цены
    std::unordered_set<int> vertexCover;

    while (true) {
        bool found = false;

        // Ищем ребро, у которого ни одна из вершин не является плотной
        for (int u = 0; u < V; ++u) {
            for (int v : graph.adj[u]) {
                if (!isTight(prices, graph, u) && !isTight(prices, graph, v)) {
                    // Увеличиваем цену ребра (u, v)
                    int maxIncrease = std::min(graph.weights[u] - prices[u], graph.weights[v] - prices[v]);
                    prices[u] += maxIncrease;
                    prices[v] += maxIncrease;
                    found = true;
                }
            }
        }

        // Если больше нет неплотных ребер, выходим из цикла
        if (!found) break;

        // Добавляем все плотные вершины в покрытие
        for (int i = 0; i < V; ++i) {
            if (isTight(prices, graph, i)) {
                vertexCover.insert(i);
            }
        }
    }

    return std::vector<int>(vertexCover.begin(), vertexCover.end());
}

int main() {
    int V;
    int E;

    std::cout << "Enter the number of vertices:";
    std::cin >> V;
    std::cout << "Enter the number of edges:";
    std::cin >> E;

    Graph graph(V, E);

    // Задание весов вершин
    std::cout << "\nEnter the vertex weights:\n\n";
    for(int i = 0; i < V; ++i) {
        int w;
        std::cout << "Enter the weights of " << i << ":";
        std::cin >> w;
        graph.setWeight(i, w);
    }

    // Добавление ребер
    std::cout << "\nEnter the edges (a pair of vertices for each edge, start from 1):\n\n";
    for(int i = 0; i < E; ++i) {
        int u;
        int v;
        std::cout << "Edge " << i + 1 << ":";
        std::cin >> u >> v;
        graph.addEdge(u - 1, v - 1);
    }

    std::vector<int> result = pricingMethod(graph);

    // Вывод результата
    std::cout << "Vertex cover: ";
    for (int v : result) {
        std::cout << v << " ";
    }
    std::cout << "\n";

    return 0;
}
