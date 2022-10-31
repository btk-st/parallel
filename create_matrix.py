import numpy as np
size = (10, 7)
filename = 'matrixA.txt'
arr2 = np.matrix(np.random.randint(-50,50,size = size))
print(arr2)

with open(filename, 'w') as f:
    f.write(' '.join(map(str, size)) + '\n')
    for line in arr2:
        np.savetxt(f, line, fmt='%d')
