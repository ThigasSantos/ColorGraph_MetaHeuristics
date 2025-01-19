#ifndef GRAPH_COLORING_LOCAL_SEARCH_H
#define GRAPH_COLORING_LOCAL_SEARCH_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <functional>

class GraphColoring_LocalSearch
{
public:
    GraphColoring_LocalSearch(int n) : n(n), numDistinctColors(0), executionTime(0), adjList(n), colors(n, -1) {}

    void addEdge(int u, int v)
    {
        if (u >= 0 && u < n && v >= 0 && v < n)
        {
            adjList[u].push_back(v);
            adjList[v].push_back(u);
        }
    }

    void initialColoring()
    {
        std::fill(colors.begin(), colors.end(), -1);

        std::vector<int> vertices(n);
        for (int i = 0; i < n; ++i)
            vertices[i] = i;
        std::sort(vertices.begin(), vertices.end(), [&](int a, int b)
                  { return adjList[a].size() > adjList[b].size(); });

        for (int v : vertices)
        {
            std::vector<bool> forbiddenColors(n, false);

            for (int u : adjList[v])
            {
                if (colors[u] != -1)
                {
                    forbiddenColors[colors[u]] = true;
                }
            }

            int color = 0;
            while (color < n && forbiddenColors[color])
            {
                color++;
            }

            colors[v] = color;
        }

        numDistinctColors = *std::max_element(colors.begin(), colors.end()) + 1;
    }

    void initialColoring_v2()
    {
        srand(static_cast<unsigned>(time(nullptr)));

        for (int i = 0; i < n; ++i)
        {
            colors[i] = rand() % 100 + 1; // Valores aleatórios de 1 a 100
        }

        numDistinctColors = *std::max_element(colors.begin(), colors.end()) + 1;
    }

    void localSearch()
    {
        initialColoring();
        std::vector<int> initialColors = colors;
        int initialCollisions = calculateCollisions();

        // Primeira melhoria para vizinhança 1
        auto resultFirstImprovement1 = neighborhood1(true);
        saveResult("Primeira melhoria (Vizinho 1)", resultFirstImprovement1, initialCollisions);

        // Melhor melhoria para vizinhança 1
        auto resultBestImprovement1 = neighborhood1(false);
        saveResult("Melhor melhoria (Vizinho 1)", resultBestImprovement1, initialCollisions);

        // Primeira melhoria para vizinhança 2
        auto resultFirstImprovement2 = neighborhood2(true);
        saveResult("Primeira melhoria (Vizinho 2)", resultFirstImprovement2, initialCollisions);

        // Melhor melhoria para vizinhança 2
        auto resultBestImprovement2 = neighborhood2(false);
        saveResult("Melhor melhoria (Vizinho 2)", resultBestImprovement2, initialCollisions);
    }

    std::vector<int> neighborhood1(bool firstImprovement)
    {
        std::vector<int> bestColors = colors;
        int bestCollisions = calculateCollisions();

        for (int startVertex = 0; startVertex < n; ++startVertex)
        {
            std::vector<int> cluster;
            std::vector<bool> visited(n, false);

            std::function<void(int)> dfs = [&](int v)
            {
                visited[v] = true;
                cluster.push_back(v);
                for (int u : adjList[v])
                {
                    if (!visited[u])
                    {
                        dfs(u);
                    }
                }
            };

            dfs(startVertex);

            std::vector<bool> colorsUsed(numDistinctColors, false);
            for (int v : cluster)
            {
                if (colors[v] != -1)
                {
                    colorsUsed[colors[v]] = true;
                }
            }

            std::vector<int> tempColors = colors;
            int newColor = 0;
            for (int v : cluster)
            {
                while (newColor < numDistinctColors && colorsUsed[newColor])
                {
                    ++newColor;
                }
                tempColors[v] = newColor;
                colorsUsed[newColor] = true;
            }

            int newCollisions = calculateCollisions(tempColors);
            if (newCollisions < bestCollisions)
            {
                bestColors = tempColors;
                bestCollisions = newCollisions;
                if (firstImprovement)
                    return bestColors;
            }
        }

        return bestColors;
    }

    std::vector<int> neighborhood2(bool firstImprovement)
    {
        std::vector<int> bestColors = colors;
        int bestCollisions = calculateCollisions();

        for (int v1 = 0; v1 < n; ++v1)
        {
            for (int v2 = 0; v2 < n; ++v2)
            {
                if (colors[v1] != colors[v2])
                {
                    int higherColorVertex = (colors[v1] > colors[v2]) ? v1 : v2;
                    int lowerColorVertex = (higherColorVertex == v1) ? v2 : v1;

                    std::vector<int> tempColors = colors;
                    tempColors[higherColorVertex] = tempColors[lowerColorVertex];

                    int newCollisions = calculateCollisions(tempColors);
                    if (newCollisions < bestCollisions)
                    {
                        bestColors = tempColors;
                        bestCollisions = newCollisions;
                        if (firstImprovement)
                            return bestColors;
                    }
                }
            }
        }

        return bestColors;
    }

    void saveResult(const std::string &description, const std::vector<int> &resultColors, int initialCollisions)
    {
        int finalCollisions = calculateCollisions(resultColors);
        int colorCount = countDistinctColors(resultColors);
        std::cout << description << ": " << colorCount << " cores diferentes, Colisões iniciais: " << initialCollisions << ", Colisões finais: " << finalCollisions << ".\n";
    }

private:
    int n;
    int numDistinctColors;
    long long executionTime;
    std::vector<std::vector<int>> adjList;
    std::vector<int> colors;

    bool isColoringValid(const std::vector<int> &tempColors) const
    {
        for (int v = 0; v < n; ++v)
        {
            for (int u : adjList[v])
            {
                if (tempColors[v] == tempColors[u])
                    return false;
            }
        }
        return true;
    }

    int calculateCollisions() const
    {
        int collisions = 0;
        for (int v = 0; v < n; ++v)
        {
            for (int u : adjList[v])
            {
                if (colors[v] == colors[u])
                {
                    ++collisions;
                }
            }
        }
        return collisions / 2;
    }

    int calculateCollisions(const std::vector<int> &tempColors) const
    {
        int collisions = 0;
        for (int v = 0; v < n; ++v)
        {
            for (int u : adjList[v])
            {
                if (tempColors[v] == tempColors[u])
                {
                    ++collisions;
                }
            }
        }
        return collisions / 2;
    }

    int countDistinctColors(const std::vector<int> &colors) const
    {
        return *std::max_element(colors.begin(), colors.end()) + 1;
    }
};

#endif // GRAPH_COLORING_LOCAL_SEARCH_H
