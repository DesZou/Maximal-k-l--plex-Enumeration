# generating a random directed simple graph with about 10 vertexes

import os
import random
import networkx as nx
import matplotlib.pyplot as plt

D = .5 # Density

if __name__ == '__main__':
    vet_num = random.randint(8, 10)

    mat = [[False] * vet_num] * vet_num

    g = nx.DiGraph()

    with open('./data/small_graph.txt', 'w') as f:
        f.write('# total vertices: {}\n'.format(vet_num))

        for r in range(len(mat)):
            for c in range(len(mat[0])):
                if r == c: continue
                mat[r][c] = True if random.random() < D else False
                if mat[r][c] is True:
                    g.add_edge(r, c)
                    f.write('{} {}\n'.format(r, c))


    nx.draw(g)
    plt.show()
