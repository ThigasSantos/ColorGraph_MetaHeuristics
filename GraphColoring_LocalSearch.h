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
        // Passo 1: Inicializar vetor de cores com -1
        std::fill(colors.begin(), colors.end(), -1);

        // Passo 2: Ordenar vértices em ordem decrescente de grau
        std::vector<int> vertices(n);
        for (int i = 0; i < n; ++i)
            vertices[i] = i;
        std::sort(vertices.begin(), vertices.end(), [&](int a, int b)
                  { return adjList[a].size() > adjList[b].size(); });

        // Passo 3: Colorir cada vértice
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

        // Contar o número de cores distintas usadas
        numDistinctColors = *std::max_element(colors.begin(), colors.end()) + 1;
    }

    void localSearch()
    {
        initialColoring();
        std::vector<int> initialColors = colors;

        // Primeira melhoria para vizinhança 1
        auto resultFirstImprovement1 = neighborhood1(true);
        saveResultColorCount("Primeira melhoria (Vizinho 1)", resultFirstImprovement1);

        // Melhor melhoria para vizinhança 1
        auto resultBestImprovement1 = neighborhood1(false);
        saveResultColorCount("Melhor melhoria (Vizinho 1)", resultBestImprovement1);

        // Primeira melhoria para vizinhança 2
        auto resultFirstImprovement2 = neighborhood2(true);
        saveResultColorCount("Primeira melhoria (Vizinho 2)", resultFirstImprovement2);

        // Melhor melhoria para vizinhança 2
        auto resultBestImprovement2 = neighborhood2(false);
        saveResultColorCount("Melhor melhoria (Vizinho 2)", resultBestImprovement2);
    }

    std::vector<int> neighborhood1(bool firstImprovement)
    {
        std::vector<int> bestColors = colors;

        // Identificar clusters (subgrafos conectados)
        for (int startVertex = 0; startVertex < n; ++startVertex)
        {
            // Cluster contendo os vértices conectados ao startVertex
            std::vector<int> cluster;
            std::vector<bool> visited(n, false);

            // Realizar uma busca em profundidade (DFS) para encontrar o cluster
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

            // Obter cores em uso no cluster
            std::vector<bool> colorsUsed(numDistinctColors, false);
            for (int v : cluster)
            {
                if (colors[v] != -1)
                {
                    colorsUsed[colors[v]] = true;
                }
            }

            // Tentar recolorir os vértices do cluster com menos cores
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

            // Contar o número de cores distintas usadas
            int newColors = countDistinctColors(tempColors);
            if (newColors < numDistinctColors)
            {
                bestColors = tempColors;
                numDistinctColors = newColors;
                if (firstImprovement)
                    return bestColors;
            }
        }

        return bestColors;
    }

    std::vector<int> neighborhood2(bool firstImprovement)
    {
        std::vector<int> bestColors = colors;
        for (int v1 = 0; v1 < n; ++v1)
        {
            for (int v2 = 0; v2 < n; ++v2)
            {
                if (colors[v1] != colors[v2])
                {
                    std::vector<int> tempColors = colors;
                    std::swap(tempColors[v1], tempColors[v2]);
                    int newColors = countDistinctColors(tempColors);
                    if (newColors < numDistinctColors)
                    {
                        bestColors = tempColors;
                        numDistinctColors = newColors;
                        if (firstImprovement)
                            return bestColors;
                    }
                }
            }
        }
        return bestColors;
    }

    void saveResultColorCount(const std::string &description, const std::vector<int> &resultColors)
    {
        int colorCount = countDistinctColors(resultColors);
        std::cout << description << ": " << colorCount << " cores diferentes.\n";
    }

    void printColors(const std::vector<int> &colors) const
    {
        // for (int i = 0; i < n; ++i)
        // {
        //     std::cout << "Vértice " << i << " -> Cor " << colors[i] << "\n";
        // }
        std::cout << "Número de cores diferentes usadas: " << countDistinctColors(colors) << "\n";
    }

private:
    int n;
    int numDistinctColors;
    long long executionTime;
    std::vector<std::vector<int>> adjList;
    std::vector<int> colors;

    bool canColor(int v, int color) const
    {
        for (int u : adjList[v])
        {
            if (colors[u] == color)
                return false;
        }
        return true;
    }

    int countDistinctColors(const std::vector<int> &colors) const
    {
        return *std::max_element(colors.begin(), colors.end()) + 1;
    }
};

#endif // GRAPH_COLORING_LOCAL_SEARCH_H
