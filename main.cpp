#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Graph.h"
#include "Faces.h"
#include<chrono>

int main() {
    setlocale(LC_ALL, "Russian");

    int dim;
    std::ifstream infile("input.txt");

    if (!infile.is_open()) {
        std::cerr << "File not found" << std::endl;
        return 1;
    }

    infile >> dim;
    std::vector<std::vector<int>> graph(dim, std::vector<int>(dim));

    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            infile >> graph[i][j];
        }
    }

    infile.close();

    auto start = std::chrono::steady_clock::now();
    Graph gr(graph);

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout <<"Time taken: "<< duration<< " milliseconds"<< std::endl;

    std::cout << "�������� ����:\n" << gr.toString() << std::endl;

    if (!gr.isConnected()) {
        std::cout << "���� �� �������� �������." << std::endl;
        return 1;
    }

    std::vector<std::pair<int, int>> bridges = gr.findBridges();
    if (!bridges.empty()) {
        std::cout << "���� �������� �����." << std::endl;
        return 1;
    }

    Faces planar = gr.getPlanarLaying();
    if (!planar.getInterior().empty() || !planar.getExternal().empty()) {
        std::cout << "���� ���������! ����� �������� �����:" << std::endl;
        std::cout << planar.toString() << std::endl;
    }
    else {
        std::cout << "���� �� ���������." << std::endl;
    }

    return 0;
}
