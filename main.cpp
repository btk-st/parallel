#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

//ограничение: число процессов <= числу строк в матрице А
//mpiexec mpi -n 5 matrixA.txt matrixB.txt matrixC.txt
//A: nxm, B: mxp, C: nxp
int main(int argc, char **argv) {
    int size = -1, rank = -1;
    int *matA = NULL, *matB = NULL, *matC = NULL;
    int *bufRowA = NULL;
    int n = NULL, m = NULL, p = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        if (argc != 4) {
            printf("Not enough files passed as arguments");
            exit(-1);
        }
        //проверим корректность размеров матриц
        FILE *fileA, *fileB;
        fileA = fopen(argv[1], "r");
        fileB = fopen(argv[2], "r");
        fscanf(fileA, "%d %d", &n, &m);
        //число процессов
        if (size > n) {
            printf("Number of rows in A matrix must be <= number of processes. rows = %d in this case", n);
            exit(-1);
        }
        int oldM = m;
        fscanf(fileB, "%d %d", &m, &p);
        if (oldM != m) {
            printf("Incorrect matrix sizes");
            exit(-1);
        }

        //заполним матрицы A и B из файла
        matA = (int *) malloc(n * m * sizeof(int));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                fscanf(fileA, "%d ", &(matA[i * m + j]));
//                printf("%d ", matA[i*m+j]);
            }
        }

        matB = (int *) malloc(m * p * sizeof(int));
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < p; j++) {
                fscanf(fileB, "%d ", &(matB[i * p + j]));
//                printf("%d ", matB[i*p+j]);
            }
        }
        fclose(fileA);
        fclose(fileB);

        //результирующая матрица C
        matC = (int *) malloc(n * p * sizeof(int));
    }
    //отправим другим процессам информацию про размеры матриц
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //1 строка матрицы A
    bufRowA = (int *) malloc(m * sizeof(int));
    //разошлем матрицу B на все процессы
    if (rank != 0) {
        matB = (int *) malloc(m * p * sizeof(int));
    }
    MPI_Bcast(matB, m * p, MPI_INT, 0, MPI_COMM_WORLD);
    //буферная строка C
    int *bufRowC = (int *) malloc(p * sizeof(int));


    //распределим строки по процессам
    MPI_Scatter(matA, m, MPI_INT, bufRowA, m, MPI_INT, 0, MPI_COMM_WORLD);
    //считаем одну из строк итоговой матрицы
    int sum = 0;
    for (int i = 0; i < p; i++) { //по столбцам B
        for (int j = 0; j < m; j++) { //по столбцам bufA
            sum += bufRowA[j] * matB[i + j * p];
        }
        bufRowC[i] = sum;
        sum = 0;
    }
    //собираем результат вычислений
    MPI_Gather(bufRowC, p, MPI_INT, matC, p, MPI_INT, 0, MPI_COMM_WORLD);


    MPI_Finalize();

    if (rank == 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                printf(" %d", matC[i*n+j]);
            }
            printf("\n");
        }
    }
}