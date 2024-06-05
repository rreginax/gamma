import random
import numpy as np

def generate_nonplanar_graph(num_nodes):
   
    adj_matrix = np.zeros((num_nodes, num_nodes), dtype=int)
    
    # Добавляем случайные ребра
    for i in range(num_nodes):
        for j in range(i+1, num_nodes):
            if random.random() < 0.5:
                adj_matrix[i][j] = 1
                adj_matrix[j][i] = 1
    
    # Добавляем перекрещивающиеся ребра, чтобы сделать граф непланарным
    for i in range(num_nodes):
        for j in range(i+2, num_nodes):
            if random.random() < 0.2:
                adj_matrix[i][j] = 1
                adj_matrix[j][i] = 1
    
    return adj_matrix

num_nodes = 30000
adj_matrix = generate_nonplanar_graph(num_nodes)


with open("nonplanar_graph.txt", "w") as f:
    for row in adj_matrix:
        f.write(" ".join(map(str, row)) + "\n")
