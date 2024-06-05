import networkx as nx
import matplotlib.pyplot as plt

# Исходный граф
edges = [(0, 1), (0, 2), (0, 3), (1, 2), (1, 3), (2, 3)]
G = nx.Graph()
G.add_edges_from(edges)

# Данные о гранях
faces = [
    [3, 2, 0],  # Внешняя грань
    [0, 3, 0, 2, 1],  
    [3, 1, 0],  
    [3, 2, 1]  
]

# Проверка на планарность
if nx.is_planar(G):
    print("Граф является планарным!")
    
    # Вычисление плоской укладки
    pos = nx.spring_layout(G)
    
    # Отрисовка графа
    plt.figure(figsize=(8, 8))
    nx.draw(G, pos, with_labels=True, node_color='blue', edge_color='black', font_size=12, font_color='white')
    
    # Отрисовка граней
    for face in faces:
        nodes = face
        plt.plot([pos[n][0] for n in nodes] + [pos[nodes[0]][0]],
                [pos[n][1] for n in nodes] + [pos[nodes[0]][1]], 'k-', linewidth=2)
    
    plt.title("Плоская укладка графа")
    plt.axis('equal')
    plt.show()
else:
    print("Граф не является планарным.")
