import networkx as nx
import matplotlib.pyplot as plt
# Определение граней плоского графа
faces = [
    [3, 2, 0],  # Внешняя грань
    [0, 3, 0, 2, 1],
    [3, 1, 0],
    [3, 2, 0],
    [3, 2, 1]
]

# Создание графа
G = nx.Graph()
G.add_edges_from([(u, v) for face in faces for u, v in zip(face, face[1:]+[face[0]])])

# Укладка графа
pos = nx.planar_layout(G)
laidVertices = [False] * len(G.nodes)

def lay_chain(chain):
    for u, v in zip(chain, chain[1:]):
        G.edges[u, v]['color'] = 'red'
        laidVertices[u] = laidVertices[v] = True
    return True

for face in faces:
    lay_chain(face)

# Визуализация
plt.figure(figsize=(10, 10))
nx.draw(G, pos, with_labels=True, node_color=[('blue' if laidVertices[v] else 'gray') for v in G.nodes],
         edge_color=[G.edges[u, v]['color'] for u, v in G.edges])
plt.title("Укладка планарного графа")
plt.show()
