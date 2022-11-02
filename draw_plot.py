import matplotlib.pyplot as plt
import numpy as np
plt.xlabel('количество потоков')
plt.ylabel('время, c')

with open('mpi_results.txt') as f:
    processes = f.readline()
    times = []
    processes = list(map(int, processes.split()))
    for time in f.readlines():
        times.append((float(time.rstrip())))
print(processes, times)


plt.plot(processes, times, marker='o', label='mpi')

with open('openmp_results.txt') as f:
    processes = f.readline()
    times = []
    processes = list(map(int, processes.split()))
    for time in f.readlines():
        times.append((float(time.rstrip())))
print(processes, times)
plt.plot(processes, times, marker='o', label='openMP')

plt.xticks(np.arange(0,41, 1.0))
plt.legend()
plt.grid()
plt.show()