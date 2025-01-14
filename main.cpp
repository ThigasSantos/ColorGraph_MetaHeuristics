#include <iostream>
#include <fstream>
#include <climits>
#include "InstanceReader.h"
#include "GraphColoring_LocalSearch.h"        // Certifique-se de incluir o arquivo correto
#include "GraphColoring_SimulatedAnnealing.h" // Adicionado para Têmpera Simulada

int main()
{
    // Lista de arquivos de entrada e saída
    std::vector<std::string> inputFiles;
    std::vector<std::string> outputFiles;

    for (int i = 1; i <= 12; i++)
    {
        inputFiles.push_back("../Instances/Instance" + std::to_string(i) + "_450_LEI.txt");
        outputFiles.push_back("../Results/output" + std::to_string(i) + "_450_LEI.txt");
    }
    for (int i = 1; i <= 5; i++)
    {
        inputFiles.push_back("../Instances/Instance" + std::to_string(i) + "_MYC.txt");
        outputFiles.push_back("../Results/output" + std::to_string(i) + "_MYC.txt");
    }
    for (int i = 1; i <= 14; i++)
    {
        inputFiles.push_back("../Instances/Instance" + std::to_string(i) + "_REG.col");
        outputFiles.push_back("../Results/output" + std::to_string(i) + "_REG.txt");
    }
    for (int i = 1; i <= 10; i++)
    {
        inputFiles.push_back("../Instances/Instance" + std::to_string(i) + "_SGB.txt");
        outputFiles.push_back("../Results/output" + std::to_string(i) + "_SGB.txt");
    }

    // Variáveis para ajustar temperatura considerando todas as instâncias
    double globalBestTemp = 0.0;
    double globalBestCoolingRate = 0.0;
    int globalBestColors = INT_MAX;

    // Processa cada arquivo
    for (size_t i = 0; i < inputFiles.size(); ++i)
    {
        const std::string &inputFilename = inputFiles[i];
        const std::string &outputFilename = outputFiles[i];

        // Ler o arquivo e carregar o grafo
        InstanceReader reader(inputFilename);
        int numVertices = reader.getNumVertices();
        const auto &edges = reader.getEdges();

        // Converter edges para std::vector<std::pair<int, int>>
        std::vector<std::pair<int, int>> edgePairs;
        for (const auto &edge : edges)
        {
            if (edge.size() == 2)
            {
                edgePairs.emplace_back(edge[0], edge[1]);
            }
        }

        // Inicializar o grafo com o número de vértices para busca local
        GraphColoring_LocalSearch localSearchGraph(numVertices);

        // Inicializar o grafo para têmpera simulada
        GraphColoring_SimulatedAnnealing simulatedAnnealingGraph(numVertices, 1000.0, 0.99, 10000);

        // Adicionar as arestas aos grafos
        for (const auto &edge : edgePairs)
        {
            localSearchGraph.addEdge(edge.first, edge.second);
            simulatedAnnealingGraph.addEdge(edge.first, edge.second);
        }

        // Abrir o arquivo de saída
        std::ofstream outputFile(outputFilename);
        if (!outputFile.is_open())
        {
            std::cerr << "Erro ao abrir o arquivo de saída " << outputFilename << ".\n";
            continue;
        }

        // Redirecionar saída padrão para o arquivo
        auto originalBuffer = std::cout.rdbuf();
        std::cout.rdbuf(outputFile.rdbuf());

        // Executar a busca local
        std::cout << "\n=== Resultados da Busca Local ===\n";
        localSearchGraph.localSearch();

        // Executar a têmpera simulada com os melhores parâmetros encontrados
        std::cout << "\n=== Resultados da Têmpera Simulada ===\n";
        std::cout << "\nVizinho 1:\n";

        // Executar a otimização de temperatura e registrar os melhores parâmetros
        std::cout << "\n=== Otimização de Temperatura ===\n";
        simulatedAnnealingGraph.optimizeTemperature(edgePairs, 1);

        double instanceBestTemp = simulatedAnnealingGraph.getBestTemp();
        double instanceBestCoolingRate = simulatedAnnealingGraph.getBestCoolingRate();
        int instanceBestColors = simulatedAnnealingGraph.getBestColors();

        if (instanceBestColors < globalBestColors)
        {
            globalBestColors = instanceBestColors;
            globalBestTemp = instanceBestTemp;
            globalBestCoolingRate = instanceBestCoolingRate;
        }
        simulatedAnnealingGraph.simulatedAnnealing(1);
        simulatedAnnealingGraph.printColors();

        std::cout << "\nVizinho 2:\n";
        // Executar a otimização de temperatura e registrar os melhores parâmetros
        std::cout << "\n=== Otimização de Temperatura ===\n";
        simulatedAnnealingGraph.optimizeTemperature(edgePairs, 2);

        instanceBestTemp = simulatedAnnealingGraph.getBestTemp();
        instanceBestCoolingRate = simulatedAnnealingGraph.getBestCoolingRate();
        instanceBestColors = simulatedAnnealingGraph.getBestColors();

        if (instanceBestColors < globalBestColors)
        {
            globalBestColors = instanceBestColors;
            globalBestTemp = instanceBestTemp;
            globalBestCoolingRate = instanceBestCoolingRate;
        }
        simulatedAnnealingGraph.simulatedAnnealing(2);
        simulatedAnnealingGraph.printColors();

        std::cout << "\nVizinho 3:\n";
        // Executar a otimização de temperatura e registrar os melhores parâmetros
        std::cout << "\n=== Otimização de Temperatura ===\n";
        simulatedAnnealingGraph.optimizeTemperature(edgePairs, 3);

        instanceBestTemp = simulatedAnnealingGraph.getBestTemp();
        instanceBestCoolingRate = simulatedAnnealingGraph.getBestCoolingRate();
        instanceBestColors = simulatedAnnealingGraph.getBestColors();

        if (instanceBestColors < globalBestColors)
        {
            globalBestColors = instanceBestColors;
            globalBestTemp = instanceBestTemp;
            globalBestCoolingRate = instanceBestCoolingRate;
        }
        simulatedAnnealingGraph.simulatedAnnealing(3);
        simulatedAnnealingGraph.printColors();

        // Restaurar a saída padrão
        std::cout.rdbuf(originalBuffer);
        outputFile.close();

        std::cout << "Resultados salvos em " << outputFilename << "\n";
    }

    // Exibir os melhores parâmetros globais
    std::cout << "\n=== Melhor Configuração Global ===\n";
    std::cout << "Temperatura Inicial: " << globalBestTemp << "\n";
    std::cout << "Taxa de Resfriamento: " << globalBestCoolingRate << "\n";

    return 0;
}
