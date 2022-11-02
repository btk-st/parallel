#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

//gcc -o openmp -fopenmp openmp.c
//./openmp.exe matrixA.txt matrixB.txt matrixC.txt <num_of_threads>
int main(int argc, char **argv) {
    int **matA, **matB, **matC;
    int n, m, p;
    int i, j, k;
    double tstart, tfinish;

    //читаем матрицы из файла
    if (argc != 5) {
        printf("Format is: ./openmp.exe matrixA.txt matrixB.txt matrixC.txt <num_of_threads>");
        exit(-1);
    }
    //проверим корректность размеров матриц
    FILE *fileA, *fileB;
    fileA = fopen(argv[1], "r");
    fileB = fopen(argv[2], "r");
    fscanf(fileA, "%d %d", &n, &m);
    int oldM = m;
    fscanf(fileB, "%d %d", &m, &p);
    if (oldM != m) {
        printf("Incorrect matrix sizes");
        exit(-1);
    }

    //заполним матрицы A и B из файла
    matA = (int **) malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        matA[i] = (int *) malloc(m * sizeof(int));
        for (j = 0; j < m; j++) {
            fscanf(fileA, "%d ", &(matA[i][j]));
        }
    }

    matB = (int **) malloc(m * sizeof(int *));
    for (i = 0; i < m; i++) {
        matB[i] = (int *) malloc(p * sizeof(int));
        for (j = 0; j < p; j++) {
            fscanf(fileB, "%d ", &(matB[i][j]));
        }
    }
    fclose(fileA);
    fclose(fileB);

    //результирующая матрица C
    matC = (int **) malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        matC[i] = (int *) malloc(p * sizeof(int));
    }
    printf("n=%d m=%d p=%d", n,m,p);
    //зафиксируем время начала вычислений
    tstart = omp_get_wtime();
    //число потоков
    int num_of_threads = strtol(argv[4], NULL, 10);
#pragma omp parallel for shared(matA, matB, matC, n, m, p) private(i, j, k) num_threads(num_of_threads)
    for (i = 0; i < n; i++) {
        for (j = 0; j < p; j++) {
            matC[i][j] = 0;
            for (k = 0; k < m; k++) matC[i][j] += matA[i][k] * matB[k][j];
        }
    }
    tfinish = omp_get_wtime();

    //пишем С в файл
    FILE *f1;
    f1 = fopen("matrixC.txt", "w");
    fprintf(f1, "%d %d\n", n, p);
    for (i = 0; i < n; i++) {
        for (j = 0; j < p; j++) {
            fprintf(f1, "%d ", matC[i][j]);
        }
        fprintf(f1, "\n");
    }
    fclose(f1);
    //вывод затраченного времени
    printf("\nTime=%lf\n", tfinish - tstart);
}