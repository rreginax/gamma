#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#include "Faces.h"

class Graph {
private:
    std::vector<std::vector<int>> matrix;
    int size;

    //Поиск простого цикла, используя DFS алгоритм
    bool dfsCycle(std::vector<int>& result, std::vector<int>& used, int parent, int v) {
        used[v] = 1;
        for (int i = 0; i < size; ++i) {
            if (i == parent || matrix[v][i] == 0) continue;
            if (used[i] == 0) {
                result.push_back(v);
                if (dfsCycle(result, used, v, i)) {
                    //Цикл найден
                    return true;
                }
                else {
                    result.pop_back();
                }
            }
            if (used[i] == 1) {
                result.push_back(v);
                std::vector<int> cycle;
                //"Выдергиваем" вершины цикла из порядка обхода
                for (int j = 0; j < result.size(); ++j) {
                    if (result[j] == i) {
                        cycle.insert(cycle.end(), result.begin() + j, result.end());
                        result = cycle;
                        return true;
                    }
                }
                return true;
            }
        }
        used[v] = 2;
        return false;
    }

    //Поиск связных компонент графа G - G', дополненного ребрами из G,
   // один из концов которых принадлежит связной компоненте, а другой G'
    void dfsSegments(std::vector<int>& used, const std::vector<bool>& laidVertexes, Graph& result, int v) {
        used[v] = 1;
        for (int i = 0; i < size; ++i) {
            if (matrix[v][i] == 1) {
                result.addEdge(v, i);
                if (used[i] == 0 && !laidVertexes[i])
                    dfsSegments(used, laidVertexes, result, i);
            }
        }
    }

    //Проверка на то, что данный сегмент содержится в данной грани
    bool isFaceContainsSegment(const std::vector<int>& face, const Graph& segment, const std::vector<bool>& laidVertexes) const {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (segment.containsEdge(i, j)) {
                    if ((laidVertexes[i] && std::find(face.begin(), face.end(), i) == face.end()) ||
                        (laidVertexes[j] && std::find(face.begin(), face.end(), j) == face.end())) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    //Считаем число граней, вмещающих данные сегменты
    int* calcNumOfFacesContainedSegments(const std::vector<std::vector<int>>& intFaces, const std::vector<int>& extFace,
        const std::vector<Graph>& segments, const std::vector<bool>& laidVertexes,
        std::vector<std::vector<int>>& destFaces) const {
        int* count = new int[segments.size()]();
        for (size_t i = 0; i < segments.size(); ++i) {
            for (const auto& face : intFaces) {
                if (isFaceContainsSegment(face, segments[i], laidVertexes)) {
                    destFaces[i] = face;
                    count[i]++;
                }
            }
            if (isFaceContainsSegment(extFace, segments[i], laidVertexes)) {
                destFaces[i] = extFace;
                count[i]++;
            }
        }
        return count;
    }

    void findBridgesUtil(int u, std::vector<bool>& visited, std::vector<int>& disc, std::vector<int>& low,
        std::vector<int>& parent, std::vector<std::pair<int, int>>& bridges) {
        static int time = 0;
        visited[u] = true;
        disc[u] = low[u] = ++time;

        for (int v = 0; v < size; ++v) {
            if (matrix[u][v]) {
                if (!visited[v]) {
                    parent[v] = u;
                    findBridgesUtil(v, visited, disc, low, parent, bridges);

                    low[u] = std::min(low[u], low[v]);

                    if (low[v] > disc[u]) {
                        bridges.push_back({ u, v });
                    }
                }
                else if (v != parent[u]) {
                    low[u] = std::min(low[u], disc[v]);
                }
            }
        }
    }

public:
    Graph(const std::vector<std::vector<int>>& m) : matrix(m), size(m.size()) {}

    Graph(int size) : size(size), matrix(size, std::vector<int>(size, 0)) {}

    std::string toString() const {
        std::ostringstream res;
        for (int i = 0; i < size; ++i) {
            for (int j = i; j < size; ++j) {
                if (matrix[i][j] == 1) {
                    res << i << " <----> " << j << "\n";
                }
            }
        }
        return res.str();
    }

    void addEdge(int k, int m) {
        matrix[k][m] = matrix[m][k] = 1;
    }

    bool containsEdge(int k, int m) const {
        return matrix[k][m] == 1;
    }

    std::vector<int> getCycle() {
        std::vector<int> cycle;
        std::vector<int> used(size, 0);
        if (!dfsCycle(cycle, used, -1, 0)) {
            return {};
        }
        return cycle;
    }
    //Получить плоскую укладку графа
   //Возвращаются все грани уложенного планарного графа
   //Если это невозможно(граф не планарный), то null
    Faces getPlanarLaying() {
        //Если граф одновершинный, то возвращаем две грани
        if (size == 1) {
            std::vector<std::vector<int>> faces;
            std::vector<int> outerFace = { 0 };
            faces.push_back(outerFace);
            faces.push_back(outerFace);
            return Faces(faces, outerFace);
        }

        //Ищем цикл, если его нет, до граф не соответствует условиям алгоритма
       //(Нет циклов => дерево => планарный)
        std::vector<int> c = getCycle();
        if (c.empty()) {
            return Faces({}, {});
        }
        //Списки граней
        std::vector<std::vector<int>> intFaces = { c };
        std::vector<int> extFace = c;
        //Массивы уже уложенных вершин и ребер соответственно
        std::vector<bool> laidVertexes(size, false);
        for (int i : c) {
            laidVertexes[i] = true;
        }
        //Укладываем найденный цикл
        std::vector<std::vector<bool>> laidEdges(size, std::vector<bool>(size, false));
        layChain(laidEdges, c, true);
        //Второй шаг алгоритма:
       //выделение множества сегментов, подсчет числа вмещающих граней,
       //выделение цепей из сегментов, укладка цепей, добавление новых граней
        while (true) {
            auto segments = getSegments(laidVertexes, laidEdges);
            if (segments.empty()) {
                break;
            }
            //Массив граней, в которые будут уложены соответствующие сегменты с минимальным числом calcNumOfFacesContainedSegments()
            std::vector<std::vector<int>> destFaces(segments.size());
            auto count = calcNumOfFacesContainedSegments(intFaces, extFace, segments, laidVertexes, destFaces);
            //Ищем минимальное число calcNumOfFacesContainedSegments()
            int mi = 0;
            for (size_t i = 0; i < segments.size(); ++i) {
                if (count[i] < count[mi])
                    mi = i;
            }
            //Если хотя бы одно ноль, то граф не планарный
            if (count[mi] == 0) {
                delete[] count;
                return Faces({}, {});
            }
            //Укладка выбранного сегмента
            //Выделяем цепь между двумя контактными вершинами
            std::vector<int> chain = segments[mi].getChain(laidVertexes);
            //Помечаем вершины цепи как уложенные
            for (int i : chain) {
                laidVertexes[i] = true;
            }
            //Укладываем соответствующие ребра цепи
            layChain(laidEdges, chain, false);
            //Целевая грань, куда будет уложен выбранный сегмент
            std::vector<int> face = destFaces[mi];
            //Новые грани, порожденные разбиением грани face выбранным сегментом
            std::vector<int> face1;
            std::vector<int> face2;
            //Ищем номера контактных вершин цепи
            int contactFirst = 0, contactSecond = 0;
            for (size_t i = 0; i < face.size(); ++i) {
                if (face[i] == chain[0]) contactFirst = i;
                if (face[i] == chain.back()) contactSecond = i;
            }
            //Находим обратную цепь(цепь, пробегаемая в обратном направлении)
            std::vector<int> reverseChain = chain;
            std::reverse(reverseChain.begin(), reverseChain.end());

            int faceSize = face.size();
            if (face != extFace) {//Если целевая грань не внешняя
                    //Укладываем прямую цепь в одну из порожденных граней,
                    //а обратную в другую в зависимости от номеров контактных вершин
                if (contactFirst < contactSecond) {
                    face1.insert(face1.end(), chain.begin(), chain.end());
                    for (int i = (contactSecond + 1) % faceSize; i != contactFirst; i = (i + 1) % faceSize) {
                        face1.push_back(face[i]);
                    }
                    face2.insert(face2.end(), reverseChain.begin(), reverseChain.end());
                    for (int i = (contactFirst + 1) % faceSize; i != contactSecond; i = (i + 1) % faceSize) {
                        face2.push_back(face[i]);
                    }
                }
                else {
                    face1.insert(face1.end(), reverseChain.begin(), reverseChain.end());
                    for (int i = (contactFirst + 1) % faceSize; i != contactSecond; i = (i + 1) % faceSize) {
                        face1.push_back(face[i]);
                    }
                    face2.insert(face2.end(), chain.begin(), chain.end());
                    for (int i = (contactSecond + 1) % faceSize; i != contactFirst; i = (i + 1) % faceSize) {
                        face2.push_back(face[i]);
                    }
                }
                //Удаляем целевую грань(она разбилась на две новые)
                //Добавляем порожденные грани в множество внутренних граней
                intFaces.erase(std::remove(intFaces.begin(), intFaces.end(), face), intFaces.end());
                intFaces.push_back(face1);
                intFaces.push_back(face2);
            }
            else {//Если целевая грань совпала с внешней
                    //Все то же самое, только одна из порожденных граней - новая внешняя грань
                std::vector<int> newOuterFace;
                if (contactFirst < contactSecond) {
                    newOuterFace.insert(newOuterFace.end(), chain.begin(), chain.end());
                    for (int i = (contactSecond + 1) % faceSize; i != contactFirst; i = (i + 1) % faceSize) {
                        newOuterFace.push_back(face[i]);
                    }
                    face2.insert(face2.end(), chain.begin(), chain.end());
                    for (int i = (contactSecond - 1 + faceSize) % faceSize; i != contactFirst; i = (i - 1 + faceSize) % faceSize) {
                        face2.push_back(face[i]);
                    }
                }
                else {
                    newOuterFace.insert(newOuterFace.end(), reverseChain.begin(), reverseChain.end());
                    for (int i = (contactFirst + 1) % faceSize; i != contactSecond; i = (i + 1) % faceSize) {
                        newOuterFace.push_back(face[i]);
                    }
                    face2.insert(face2.end(), reverseChain.begin(), reverseChain.end());
                    for (int i = (contactFirst - 1 + faceSize) % faceSize; i != contactSecond; i = (i - 1 + faceSize) % faceSize) {
                        face2.push_back(face[i]);
                    }
                }
                //Удаляем старые, добавляем новые
                intFaces.erase(std::remove(intFaces.begin(), intFaces.end(), extFace), intFaces.end());
                intFaces.push_back(newOuterFace);
                intFaces.push_back(face2);
                extFace = newOuterFace;
            }
        }
        return Faces(intFaces, extFace);
    }

    std::vector<Graph> getSegments(const std::vector<bool>& laidVertexes, const std::vector<std::vector<bool>>& edges) {
        std::vector<Graph> segments;
        for (int i = 0; i < size; ++i) {
            for (int j = i + 1; j < size; ++j) {
                if (matrix[i][j] == 1 && !edges[i][j] && laidVertexes[i] && laidVertexes[j]) {
                    Graph t(size);
                    t.addEdge(i, j);
                    segments.push_back(t);
                }
            }
        }

        std::vector<int> used(size, 0);
        for (int i = 0; i < size; ++i) {
            if (used[i] == 0 && !laidVertexes[i]) {
                Graph res(size);
                dfsSegments(used, laidVertexes, res, i);
                segments.push_back(res);
            }
        }
        return segments;
    }

    std::vector<int> getChain(const std::vector<bool>& laidVertexes) {
        std::vector<int> result;
        for (int i = 0; i < size; ++i) {
            if (laidVertexes[i]) {
                bool inGraph = false;
                for (int j = 0; j < size; ++j) {
                    if (containsEdge(i, j)) {
                        inGraph = true;
                        break;
                    }
                }
                if (inGraph) {
                    dfsChain(result, laidVertexes, i);
                    break;
                }
            }
        }
        return result;
    }

    //Поиск цепи в выбранном сегменте, используя DFS алгоритм
    void dfsChain(std::vector<int>& chain, const std::vector<bool>& laidVertexes, int v) {
        std::vector<int> used(size, 0);
        used[v] = 1;
        chain.push_back(v);
        for (int i = 0; i < size; ++i) {
            if (matrix[v][i] == 1 && used[i] == 0) {
                if (!laidVertexes[i]) {
                    dfsChain(chain, laidVertexes, i);
                }
                else {
                    chain.push_back(i);
                }
                return;
            }
        }
    }
    //Укладка цепи, описание матрицы смежности
    static void layChain(std::vector<std::vector<bool>>& result, const std::vector<int>& chain, bool cyclic) {
        for (size_t i = 0; i < chain.size() - 1; ++i) {
            result[chain[i]][chain[i + 1]] = true;
            result[chain[i + 1]][chain[i]] = true;
        }
        if (cyclic) {
            result[chain[0]][chain.back()] = true;
            result[chain.back()][chain[0]] = true;
        }
    }

    std::vector<std::pair<int, int>> findBridges() {
        std::vector<std::pair<int, int>> bridges;
        std::vector<bool> visited(size, false);
        std::vector<int> disc(size, -1);
        std::vector<int> low(size, -1);
        std::vector<int> parent(size, -1);

        for (int i = 0; i < size; ++i) {
            if (!visited[i]) {
                findBridgesUtil(i, visited, disc, low, parent, bridges);
            }
        }
        return bridges;
    }

    bool isConnected() {
        std::vector<bool> visited(size, false);
        std::queue<int> q;
        q.push(0);
        visited[0] = true;
        int count = 1;

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            for (int i = 0; i < size; ++i) {
                if (matrix[v][i] == 1 && !visited[i]) {
                    visited[i] = true;
                    q.push(i);
                    count++;
                }
            }
        }

        return count == size;
    }
};

#endif // GRAPH_H