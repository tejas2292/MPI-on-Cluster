#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"
#include "matrixOps.h"

#define FILE_IN_NAME "infile"
#define FILE_OUT_NAME "outfile"
#define MASTER 0
#define SIZE (sizeof(int) * rowsA) + (2 * sizeof(int))

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    double startTime = MPI_Wtime();
    srand((unsigned)time(NULL));

    int tasksNum; /*Number of tasks in the communicator*/
    int taskId;   /*Task's rank in the communicator*/
    int rowsA, colsA, rowsB, colsB;

    MPI_Comm_size(MPI_COMM_WORLD, &tasksNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
    MPI_Status status;

    if (tasksNum == 1)
    {
        printf("You are using 1 processor, execute the sequential version with file \"seq\"\n");
        exit(EXIT_FAILURE);
    }

    char pathname[50];
    sprintf(pathname, "../test/%s", FILE_IN_NAME);
    FILE *inFile = fopen(pathname, "r");

    if (inFile == NULL)
    {
        printf("Error opening the file\n");
        exit(EXIT_FAILURE);
    }

    fscanf(inFile, " %d %d %d %d", &rowsA, &colsA, &rowsB, &colsB); /*Reading rows and columns of matrix A and matrix B from the first row of the file "infile"*/

    if (colsA != rowsB)
    {
        printf("Cannot compute A X B -> matrices have to be of the form A[m][n] B[n][l], m can be equal to l\n");
        exit(EXIT_FAILURE);
    }
    else if (tasksNum > rowsA * colsB)
    {
        printf("The problem size is too small for the execution in parallel with %d tasks\n", tasksNum);
        exit(EXIT_FAILURE);
    }

    const int portion = rowsA * colsB / tasksNum;
    const int reminder = (rowsA * colsB) % tasksNum;

    int *recvBuf = malloc(SIZE); /*Buffer to store: <row indexes of matrix A assigned to the task><offset from column 0 of matrix B><number of elements of product matrix to compute>*/
    int rows;                    /*Number of rows of matrix A to compute the assigned elements*/

    if (taskId == MASTER)
    {

        int elements = 0;   /*Current elements collected*/
        int taskElements;   /*Number of elements assigned to the task*/
        int taskRank = 0;   /*Current task rank*/
        int colBOffset = 0; /*Offset from column 0 of matrix B*/
        int rowsCount = 0;
        int *indexBuf = malloc(SIZE);
        int index = 0;

        taskElements = (taskRank < reminder) ? portion + 1 : portion;

        for (int row = 0; row < rowsA; row++)
        {
            rowsCount++;

            (taskRank == 0) ? (recvBuf[index++] = row) : (indexBuf[index++] = row);

            for (int col = 0; col < colsB; col++)
            {
                elements++;

                if (elements == taskElements)
                {

                    if (taskRank == 0)
                    {
                        rows = rowsCount;
                        recvBuf[rowsCount] = colBOffset;
                        recvBuf[rowsCount + 1] = elements;
                    }
                    else
                    {
                        MPI_Ssend(&rowsCount, 1, MPI_INT, taskRank, 0, MPI_COMM_WORLD);
                        indexBuf[rowsCount] = colBOffset;
                        indexBuf[rowsCount + 1] = elements;
                        MPI_Ssend(indexBuf, rowsCount + 2, MPI_INT, taskRank, 0, MPI_COMM_WORLD);
                    }

                    colBOffset = (colBOffset + elements) % colsB;
                    index = 0;

                    if (col < colsB - 1)
                    { /*Save row index for next task before moving on with row++*/
                        indexBuf[index++] = row;
                        rowsCount = 1;
                    }
                    else
                        rowsCount = 0;

                    elements = 0;
                    taskRank++;

                    taskElements = (taskRank < reminder) ? portion + 1 : portion;
                }
            }
        }
        free(indexBuf);
    }
    else
    {
        MPI_Recv(&rows, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(recvBuf, rows + 2, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);
    }

    int offset = recvBuf[rows], elements = recvBuf[rows + 1];
    int **partMatrixA = allocateMatrix(rows, colsA);
    int **partMatrixB = allocateMatrix(rows, colsB);

    for (int i = 0, j = 0, next; i < rowsA; i++)
    { /*Reading selected rows from matrix A*/
        if (i == recvBuf[j] && j < rows)
        {
            for (int k = 0; k < colsA; k++)
                fscanf(inFile, " %d", &partMatrixA[j][k]);
            j++;
        }
        else
        {
            for (int k = 0; k < colsA; k++)
                fscanf(inFile, " %d", &next);
        }
    }

    for (int i = 0, j = 0, next; i < rowsB; i++)
    { /*Reading selected rows from matrix B*/
        if (i == recvBuf[j] && j < rows)
        {
            for (int k = 0; k < colsB; k++)
                fscanf(inFile, " %d", &partMatrixB[j][k]);
            j++;
        }
        else
        {
            for (int k = 0; k < colsB; k++)
                fscanf(inFile, " %d", &next);
        }
    }
    int *sendBuf = addMatricesretVect(partMatrixA, partMatrixB, rows, colsA, rowsB, colsB, offset, elements); /*Buffer used to store computed values to send to MASTER*/
    int *recvGatBuf = malloc(sizeof(int) * elements * tasksNum);                                              /*Buffer used to gather all partial results from all workers*/
    int recvCounts[tasksNum];
    int displs[tasksNum];
    displs[0] = 0;

    for (int taskRank = 0; taskRank < tasksNum; taskRank++)
    {
        recvCounts[taskRank] = (taskRank < reminder) ? portion + 1 : portion;
        displs[taskRank + 1] = displs[taskRank] + recvCounts[taskRank];
    }

    MPI_Gatherv(sendBuf, elements, MPI_INT, recvGatBuf, recvCounts, displs, MPI_INT, MASTER, MPI_COMM_WORLD);

    if (taskId == MASTER)
    {
        sprintf(pathname, "../test/%s", FILE_OUT_NAME);
        FILE *outFile = fopen(pathname, "w+");
        fprintf(outFile, "%d %d\n", rowsA, colsB);
        for (int i = 0; i < rowsA * colsB; i++)
        {
            fprintf(outFile, "%d ", recvGatBuf[i]);
            if ((i + 1) % colsB == 0)
                fprintf(outFile, "\n");
        }
        fclose(outFile);
    }

    fclose(inFile);
    free(recvBuf);
    free(recvGatBuf);
    free(sendBuf);
    free(partMatrixB);
    free(partMatrixA);

    double endTime = MPI_Wtime();

    if (taskId == MASTER)
    {
        printf("The computation took %.2f seconds\nOpen the file \"outfile\" in test folder to see the result matrix\n", endTime - startTime);
    }
    MPI_Finalize();
    return 0;
}
