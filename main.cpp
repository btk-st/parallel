#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

//ограничение: число процессов <= числу строк в матрице А
//mpiexec mpi -n 5 matrixA.txt matrixB.txt matrixC.txt
//не работает если число строк в последнем процессе > чем в остальных
//A: nxm, B: mxp, C: nxp
int main(int argc, char **argv) {
    double tstart=NULL, tfinish=NULL;
    int size = NULL, rank = NULL;
    int *matA = NULL, *matB = NULL, *matC = NULL;
    int *bufRowA = NULL, *bufRowC = NULL;
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
    tstart = MPI_Wtime();
    //отправим другим процессам информацию про размеры матриц
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //разошлем матрицу B на все процессы
    if (rank != 0) {
        matB = (int *) malloc(m * p * sizeof(int));
    }
    MPI_Bcast(matB, m * p, MPI_INT, 0, MPI_COMM_WORLD);

    //число строк на процесс
    int rows_per_process;
    int rows_in_last_process;
    //если делятся нацело - распределяем равномерно
    if (n % size == 0) {
        rows_per_process = n / size;
        rows_in_last_process = rows_per_process;
    } else {
        //если нет, то последний процесс получает меньшее число строк на обработку
        rows_per_process = n / (size-1);
        rows_in_last_process = n - (size - 1) * rows_per_process;
    }

    int processRows;
    //выделим память под буферную строку с учетом номера процесса
    if (rank == size - 1) {
        processRows = rows_in_last_process;
    } else {
        processRows = rows_per_process;
    }
    bufRowC = (int *) malloc(p * processRows * sizeof(int));
    bufRowA = (int *) malloc(m * processRows * sizeof(int));


//    printf("%d: rows_in_last:%d, rows_per_process: %d,  processRows=%d", rank, rows_in_last_process, rows_per_process,processRows);

    //распределим строки по процессам
    MPI_Scatter(matA, processRows * m, MPI_INT, bufRowA, processRows * m, MPI_INT, 0, MPI_COMM_WORLD);
//    printf("\nfirst row element: %d", bufRowA[0]);
    //считаем одну из строк итоговой матрицы
    int sum = 0;
    for (int rowA = 0; rowA < processRows; rowA++) //по строкам bufA
        for (int i = 0; i < p; i++) { //по столбцам B
            for (int j = 0; j < m; j++) { //по столбцам bufA
                sum += bufRowA[rowA * m + j] * matB[i + j * p];
            }
            bufRowC[rowA * p + i] = sum;
            sum = 0;
        }
    //собираем результат вычислений
    MPI_Gather(bufRowC, processRows*p, MPI_INT, matC, processRows*p, MPI_INT, 0, MPI_COMM_WORLD);

    tfinish = MPI_Wtime();
    MPI_Finalize();

    if (rank == 0) {
        printf("%f %f %f", tstart, tfinish, tfinish-tstart);
//        for (int i = 0; i < n; i++) {
//            for (int j = 0; j < p; j++) {
//                printf(" %d", matC[i*n+j]);
//            }
//            printf("\n");
//        }
        //save C to file
        FILE *f1;
        f1=fopen("matrixC.txt","w");
        fprintf(f1,"%d %d\n", n, p);
        for (int i=0;i<n;i++)
        {
            for (int j=0;j<p;j++)
            {
                fprintf(f1,"%d ", matC[i*p+j]);
            }
            fprintf(f1,"\n");
        }
        fclose(f1);
    }
}