#ifndef GRAPH_COLORING_SIMULATED_ANNEALING_H
#define GRAPH_COLORING_SIMULATED_ANNEALING_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

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

    void simulatedAnnealing()
    {
        initialColoring();
        std::vector<int> bestColorsVec = colors;
        int bestCost = numDistinctColors;

        double temperature = initialTemp;
        srand(static_cast<unsigned>(time(nullptr)));

        for (int iter = 0; iter < maxIterations; ++iter)
        {
            std::vector<int> newColors = generateNeighbor();
            int newCost = countDistinctColors(newColors);

            if (newCost < bestCost || acceptWorseSolution(bestCost, newCost, temperature))
            {
                colors = newColors;
                bestCost = newCost;
                bestColorsVec = colors;
            }

            temperature *= coolingRate;
        }

        colors = bestColorsVec;
        numDistinctColors = bestCost;
    }

    void printColors() const
    {
        // for (int i = 0; i < n; ++i)
        // {
        //     std::cout << "Vértice " << i << " -> Cor " << colors[i] << "\n";
        // }
        std::cout << "Número de cores diferentes usadas: " << numDistinctColors << "\n";
    }

    int getDistinctColors() const
    {
        return numDistinctColors;
    }

    void optimizeTemperature(const std::vector<std::pair<int, int>>& edges)
    {
        bestTemp = 0.0;
        bestCoolingRate = 0.0;
        bestColors = n; // Número máximo inicial
        const int maxIterationsForTest = 1000; // Iterações fixas para avaliação

        for (double initialTempTest = 100.0; initialTempTest <= 2000.0; initialTempTest += 100.0)
        {
            for (double coolingRateTest = 0.90; coolingRateTest <= 0.99; coolingRateTest += 0.01)
            {
                GraphColoring_SimulatedAnnealing tempGraph(n, initialTempTest, coolingRateTest, maxIterationsForTest);

                // Adicionar arestas ao grafo temporário
                for (const auto& edge : edges)
                {
                    tempGraph.addEdge(edge.first, edge.second);
                }

                // Executar a têmpera simulada
                tempGraph.simulatedAnnealing();

                // Avaliar o resultado
                int currentColors = tempGraph.getDistinctColors();
                if (currentColors < bestColors)
                {
                    bestColors = currentColors;
                    bestTemp = initialTempTest;
                    bestCoolingRate = coolingRateTest;
                }
            }
        }

        std::cout << "\n=== Melhor Configuração Encontrada ===\n";
        std::cout << "Temperatura Inicial: " << bestTemp << "\n";
        std::cout << "Taxa de Resfriamento: " << bestCoolingRate << "\n";
    }

    double getBestTemp() const { return bestTemp; }
    double getBestCoolingRate() const { return bestCoolingRate; }
    int getBestColors() const { return bestColors; }

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

    std::vector<int> generateNeighbor()
    {
        std::vector<int> newColors = colors;
        if (rand() % 2 == 0)
        {
            // Vizinhança 1: Alterar a cor de um vértice
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
            // Vizinhança 2: Trocar cores de dois vértices
            int v1 = rand() % n;
            int v2 = rand() % n;
            std::swap(newColors[v1], newColors[v2]);
        }
        return newColors;
    }
};

#endif // GRAPH_COLORING_SIMULATED_ANNEALING_H
