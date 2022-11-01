with open('mpi_results.txt') as f:
    processes = f.readline()
    times = []
    processes = list(map(int, processes.split()))
    for time in f.readlines():
        times.append((float(time.rstrip())))
print(processes, times)

import matplotlib.pyplot as plt
plt.xlabel('количество потоков')
plt.ylabel('время')
plt.plot(processes, times, marker='o')
plt.show()