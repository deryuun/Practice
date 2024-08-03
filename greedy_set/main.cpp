#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>

int main() {
    int n, m;
    std::cout << "Enter the number of elements (n): ";
    std::cin >> n;
    std::cout << "Enter the number of sets (m): ";
    std::cin >> m;

    std::vector<std::set<int>> sets(m);
    std::vector<int> setSizes(m);
    std::unordered_set<int> universalSet; // Универсальное множество (все элементы)

    // Заполнение универсального множества
    for (int i = 0; i < n; ++i) {
        universalSet.insert(i);
    }

    // Ввод множеств
    std::cout << "Enter the elements of each set (space-separated, terminated by -1):" << std::endl;
    for (int i = 0; i < m; ++i) {
        std::cout << "Set " << i + 1 << ": ";
        int element;
        while (std::cin >> element && element != -1) {
            sets[i].insert(element);
        }
        setSizes[i] = sets[i].size(); // Запись размера множества
    }

    std::unordered_set<int> covered;
    std::vector<int> cover;

    // Жадный алгоритм покрытия множеств
    while (covered.size() < n) {
        int bestSet = -1;
        int maxCover = 0;
        // Поиск множества, покрывающего наибольшее количество непокрытых элементов
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

        // Если невозможно покрыть все элементы
        if (bestSet == -1) {
            std::cout << "Cannot cover all elements." << std::endl;
            return 1;
        }

        // Добавление выбранного множества в покрытие
        cover.push_back(bestSet);
        for (int element : sets[bestSet]) {
            covered.insert(element);
        }
    }

    std::cout << "Selected sets to cover all elements: ";
    for (int setIndex : cover) {
        std::cout << setIndex + 1 << " ";
    }
    std::cout << std::endl;

    return 0;
}
