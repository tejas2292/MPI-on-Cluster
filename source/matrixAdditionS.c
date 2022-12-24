#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define NUM_ROWS 2048
#define NUM_COLS 2048
#define MAX_RAND 5

int main(int argc, char *argv[])
{
    clock_t startTime, endTime;
    startTime = clock();
    srand((unsigned)time(NULL));

    int offset, i, j;
    int A[NUM_ROWS][NUM_COLS], B[NUM_ROWS][NUM_COLS], C[NUM_ROWS][NUM_COLS];
    FILE *fp;
    char *filename_A = "matAlarge.txt";
    char *filename_B = "matBlarge.txt";
    char *filename_C = "additionResult.txt";
    char pathname[80];

    /* read matrix A from file */

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

    /* read matrix B from file */

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

    /* add matrix A and B */
    for (i = 0; i < NUM_ROWS; i++)
    {
        for (j = 0; j < NUM_COLS; j++)
        {
            C[i][j] = A[i][j] + B[i][j];
        }
    }

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

    endTime = clock();
    printf("The computation took %.2lf seconds\nOpen file \"additionResult\" in test folder to see the result matrix\n", (double)(endTime - startTime) / CLOCKS_PER_SEC);
    return 0;
}