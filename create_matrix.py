import numpy as np
size = (1000, 500)
filename = 'matrixB.txt'
arr2 = np.matrix(np.random.randint(-50,50,size = size))

with open(filename, 'w') as f:
    f.write(' '.join(map(str, size)) + '\n')
    for line in arr2:
        np.savetxt(f, line, fmt='%d')
