#ifndef INSTANCE_READER_H
#define INSTANCE_READER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class InstanceReader
{
public:
    InstanceReader(const std::string &filename)
    {
        readFile(filename);
    }

    void printEdges() const
    {
        std::cout << "Edges:\n";
        for (const auto &edge : edges)
        {
            std::cout << edge[0] << " -- " << edge[1] << "\n";
        }
        std::cout << "Number of vertices: " << numVertices << "\n";
        std::cout << "Number of edges: " << numEdges << "\n";
    }

    int getNumVertices() const { return numVertices; }
    int getNumEdges() const { return numEdges; }
    const std::vector<std::vector<int>> &getEdges() const { return edges; }

private:
    int numVertices = 0;
    int numEdges = 0;
    std::vector<std::vector<int>> edges;

    void readFile(const std::string &filename)
    {
        std::ifstream file(filename);
        std::string line;

        if (!file.is_open())
        {
            std::cerr << "Could not open the file.\n";
            return;
        }

        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == 'c')
            {
                continue; // Ignora linhas comentadas.
            }

            if (line[0] == 'p')
            {
                std::istringstream ss(line);
                char p;
                std::string text;
                ss >> p >> text >> numVertices >> numEdges;
            }
            else if (line[0] == 'e')
            {
                std::istringstream ss(line);
                char e;
                int v1, v2;
                ss >> e >> v1 >> v2;       // Ignora o 'e' e lê os dois números
                edges.push_back({v1, v2}); // Armazena o par de vértices em edges
            }
        }

        file.close();
        printEdges();
    }
};

#endif
