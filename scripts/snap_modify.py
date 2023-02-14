
# use for modify data from http://snap.stanford.edu/

import os
import re

Idx = 0
M = {}

def trans(a):
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
                for line in f.readlines():
                    if line.strip().startswith('#'):
                        g.write(line)
                        continue
                    x, y = re.findall("\d+", line)
                    if x == y:
                        continue
                    g.write(str(trans(x)) + " " + str(trans(y)) + "\n")
                    
    