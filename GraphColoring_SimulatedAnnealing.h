#ifndef GRAPH_COLORING_SIMULATED_ANNEALING_H
#define GRAPH_COLORING_SIMULATED_ANNEALING_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <functional>

class GraphColoring_SimulatedAnnealing
{
public:
    GraphColoring_SimulatedAnnealing(int n, double initialTemp, double coolingRate, int maxIterations)
        : n(n), initialTemp(initialTemp), coolingRate(coolingRate), maxIterations(maxIterations),
          numDistinctColors(0), adjList(n), colors(n, -1), bestTemp(initialTemp), bestCoolingRate(coolingRate), bestColors(n) {}

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

    void simulatedAnnealing(int neighborhoodType)
    {
        initialColoring();
        std::vector<int> bestColorsVec = colors;
        int bestCost = numDistinctColors;
        int initialCollisions = calculateCollisions();
        int bestCollisions = initialCollisions;

        double temperature = initialTemp;
        srand(static_cast<unsigned>(time(nullptr)));

        for (int iter = 0; iter < maxIterations; ++iter)
        {
            std::vector<int> newColors;

            if (neighborhoodType == 1)
            {
                newColors = generateNeighbor1();
            }
            else if (neighborhoodType == 2)
            {
                newColors = generateNeighbor2();
            }
            else if (neighborhoodType == 3)
            {
                newColors = generateNeighbor3();
            }

            int newCollisions = calculateCollisions(newColors);
            int newCost = countDistinctColors(newColors);

            if ((newCost < bestCost) ||
                (newCost == bestCost && newCollisions < bestCollisions) ||
                acceptWorseSolution(bestCost, newCost, temperature))
            {
                colors = newColors;
                bestCost = newCost;
                bestCollisions = newCollisions;
                bestColorsVec = colors;
            }

            temperature *= coolingRate;
        }

        colors = bestColorsVec;
        numDistinctColors = bestCost;
        std::cout << "Colisões iniciais: " << initialCollisions << ", Colisões finais: " << bestCollisions << "\n";
    }

    void printColors() const
    {
        int finalCollisions = calculateCollisions();
        std::cout << "Número de cores diferentes usadas: " << numDistinctColors << "\n";
        std::cout << "Colisões finais: " << finalCollisions << "\n";
    }

private:
    int n;
    double initialTemp;
    double coolingRate;
    int maxIterations;
    int numDistinctColors;
    double bestTemp;
    double bestCoolingRate;
    int bestColors;
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

    bool acceptWorseSolution(int currentCost, int newCost, double temperature) const
    {
        if (temperature <= 0)
            return false;
        double probability = std::exp((currentCost - newCost) / temperature);
        return (static_cast<double>(rand()) / RAND_MAX) < probability;
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

    int calculateCollisions(const std::vector<int> &newColors) const
    {
        int collisions = 0;
        for (int v = 0; v < n; ++v)
        {
            for (int u : adjList[v])
            {
                if (newColors[v] == newColors[u])
                {
                    ++collisions;
                }
            }
        }
        return collisions / 2;
    }

    std::vector<int> generateNeighbor1()
    {
        std::vector<int> newColors = colors;

        std::vector<bool> visited(n, false);
        std::vector<int> cluster;

        int startVertex = rand() % n;

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
            if (newColors[v] != -1)
            {
                colorsUsed[newColors[v]] = true;
            }
        }

        int newColor = 0;
        for (int v : cluster)
        {
            while (newColor < numDistinctColors && colorsUsed[newColor])
            {
                ++newColor;
            }
            newColors[v] = newColor;
            colorsUsed[newColor] = true;
        }

        // Garantir que o resultado não é pior que o inicial
        if (calculateCollisions(newColors) > calculateCollisions())
        {
            return colors; // Retorna as cores originais se o novo estado for pior
        }

        return newColors;
    }

    std::vector<int> generateNeighbor2()
    {
        std::vector<int> newColors = colors;

        int v1 = rand() % n;
        int v2 = rand() % n;

        if (v1 != v2)
        {
            int higherColorVertex = (colors[v1] > colors[v2]) ? v1 : v2;
            int lowerColorVertex = (higherColorVertex == v1) ? v2 : v1;

            if (canColor(higherColorVertex, colors[lowerColorVertex]))
            {
                newColors[higherColorVertex] = colors[lowerColorVertex];
            }
        }

        return newColors;
    }

    std::vector<int> generateNeighbor3()
    {
        std::vector<int> newColors = colors;
        if (rand() % 2 == 0)
        {
            int v = rand() % n;
            for (int c = 0; c < numDistinctColors; ++c)
            {
                if (canColor(v, c))
                {
                    newColors[v] = c;
                    break;
                }
            }
        }
        else
        {
            int v1 = rand() % n;
            int v2 = rand() % n;

            if (v1 != v2)
            {
                int higherColorVertex = (colors[v1] > colors[v2]) ? v1 : v2;
                int lowerColorVertex = (higherColorVertex == v1) ? v2 : v1;

                if (canColor(higherColorVertex, colors[lowerColorVertex]))
                {
                    newColors[higherColorVertex] = colors[lowerColorVertex];
                }
            }
        }
        return newColors;
    }
};

#endif // GRAPH_COLORING_SIMULATED_ANNEALING_H
