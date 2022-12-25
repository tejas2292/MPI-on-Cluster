#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrixOps.h"
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_RAND 256
#define DIR_NAME "test"
#define FILE_NAME "infile"
#define FILE_NAMEA "matAlarge.txt"
#define FILE_NAMEB "matBlarge.txt"

int main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));

    if (argc != 5)
    {
        printf("Bad usage: ./a.out <rowsA> <colsA> <rowsB> <colsB>\n");
        exit(EXIT_FAILURE);
    }

    const int rowsA = atoi(argv[1]);
    const int colsA = atoi(argv[2]);
    const int rowsB = atoi(argv[3]);
    const int colsB = atoi(argv[4]);

    printf("Writing two matrices on file \"%s\" matAlarge and matBlarge...\n", FILE_NAME);
    printf("Matrix A: %d rows %d columns\n", rowsA, colsA);
    printf("Matrix B: %d rows %d columns\n", rowsB, colsB);

    char pathname[50];
    sprintf(pathname, "../%s", DIR_NAME);

    struct stat st = {0};

    if (stat("../test/", &st) == -1)
    { /*Create the test directory if not exists*/
        mkdir(pathname, 0700);
    }

    sprintf(pathname, "../test/%s", FILE_NAME);
    FILE *file = fopen(pathname, "w");

    if (file == NULL)
    {
        perror("Error opening the file");
    }
    else
    {
        fprintf(file, "%d %d %d %d\n", rowsA, colsA, rowsB, colsB);
        writeRandMatrixOnFile(file, rowsA, colsA, MAX_RAND);
        fprintf(file, "\n");
        writeRandMatrixOnFile(file, rowsB, colsB, MAX_RAND);
    }

    fclose(file);
    return 0;
}
