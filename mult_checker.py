import numpy as np
with open('matrixC.txt') as f:
    size = f.readline()
    n, m = map(int, size.split())
c = np.loadtxt('matrixC.txt', usecols=range(m), skiprows=1)

with open('matrixA.txt') as f:
    size = f.readline()
    n, m = map(int, size.split())
a = np.loadtxt('matrixA.txt', usecols=range(m), skiprows=1)

with open('matrixB.txt') as f:
    size = f.readline()
    n, m = map(int, size.split())
b = np.loadtxt('matrixB.txt', usecols=range(m), skiprows=1)


# print(np.matmul(a, b))
# print(c)
print("matrix equals? ", np.array_equal(np.matmul(a,b), c))
