#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

#define NUM_ROWS 2048
#define NUM_COLS 2048

int main(int argc, char *argv[])
{
    int my_rank, num_procs;
    int row_start, row_end;
    int offset, i, j;
    int A[NUM_ROWS][NUM_COLS], B[NUM_ROWS][NUM_COLS], C[NUM_ROWS][NUM_COLS];
    int status;
    int total;
    MPI_Status stat;
    double start, end;
    FILE *fp;
    char *filename_A = "matAlarge.txt";
    char *filename_B = "matBlarge.txt";
    char *filename_C = "additionResult.txt";
    char pathname[50];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    start = MPI_Wtime();

    /* read matrix A from file */
    if (my_rank == 0)
    {
        sprintf(pathname, "../test/%s", filename_A);
        if ((fp = fopen(pathname, "r")) == NULL)
        {
            printf("Can't open file %s\n", filename_A);
            exit(1);
        }
        else
        {
            for (i = 0; i < NUM_ROWS; i++)
            {
                for (j = 0; j < NUM_COLS; j++)
                {
                    fscanf(fp, "%d", &A[i][j]);
                }
            }
            fclose(fp);
        }
    }

    /* read matrix B from file */
    if (my_rank == 0)
    {
        sprintf(pathname, "../test/%s", filename_B);
        if ((fp = fopen(pathname, "r")) == NULL)
        {
            printf("Can't open file %s\n", filename_B);
            exit(1);
        }
        else
        {
            for (i = 0; i < NUM_ROWS; i++)
            {
                for (j = 0; j < NUM_COLS; j++)
                {
                    fscanf(fp, "%d", &B[i][j]);
                }
            }
            fclose(fp);
        }
    }

    /* scatter matrix A to all processes */
    row_start = my_rank * (NUM_ROWS / num_procs);
    row_end = (my_rank + 1) * (NUM_ROWS / num_procs);

    int local_matrix1[NUM_ROWS / num_procs][NUM_COLS];
    int local_matrix2[NUM_ROWS / num_procs][NUM_COLS];
    int local_result[NUM_ROWS / num_procs][NUM_COLS];

    MPI_Scatter(A, (NUM_ROWS * NUM_COLS / num_procs), MPI_INT, local_matrix1, (NUM_ROWS * NUM_COLS / num_procs), MPI_INT, 0, MPI_COMM_WORLD);

    /* scatter matrix B to all processes */
    MPI_Scatter(B, (NUM_ROWS * NUM_COLS / num_procs), MPI_INT, local_matrix2, (NUM_ROWS * NUM_COLS / num_procs), MPI_INT, 0, MPI_COMM_WORLD);

    /* add matrix A and B */
    for (i = 0; i < NUM_ROWS / num_procs; i++)
    {
        for (j = 0; j < NUM_COLS; j++)
        {
            local_result[i][j] = local_matrix1[i][j] + local_matrix2[i][j];
        }
    }

    /* gather matrix C from all processes */
    MPI_Gather(local_result, (NUM_ROWS * NUM_COLS / num_procs), MPI_INT, C, (NUM_ROWS * NUM_COLS / num_procs), MPI_INT, 0, MPI_COMM_WORLD);

    /* write matrix C to file */
    if (my_rank == 0)
    {
        sprintf(pathname, "../test/%s", filename_C);
        if ((fp = fopen(pathname, "w")) == NULL)
        {
            printf("Can't open file %s\n", filename_C);
            exit(1);
        }
        else
        {
            for (i = 0; i < NUM_ROWS; i++)
            {
                for (j = 0; j < NUM_COLS; j++)
                {
                    fprintf(fp, "%d\t", C[i][j]);
                }
                fprintf(fp, "\n");
            }
            fclose(fp);
        }
    }
    end = MPI_Wtime();

    if (my_rank == 0)
    {
        printf("Time taken = %.2f seconds\n", end - start);
    }
    MPI_Finalize();
    return 0;
}