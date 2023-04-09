
# use for modify data from http://snap.stanford.edu/

import os
import re

Idx = 0
M = {}

def reindex(a):
    if M.get(a) is None:
        global Idx
        M[a] = Idx
        Idx += 1
    return M[a]

if __name__ == '__main__':
    input_path = "./data/SNAP/"
    output_path = "./data/SNAP_modified/"

    for filename in os.listdir(input_path):
        with open(input_path + filename, "r") as f:
            with open(output_path + filename, "w") as g:
                Idx = 0
                M = {}
                R = {}
                for line in f.readlines():
                    if line.strip().startswith('#'):
                        g.write(line)
                        continue
                    x, y = re.findall("\d+", line)[0: 2]
                    x = reindex(x)
                    y = reindex(y)
                    if x == y or R.get((x, y)) is True:
                        continue
                    g.write(f"{x} {y}\n")
                    R[(x, y)] = True
                    
    