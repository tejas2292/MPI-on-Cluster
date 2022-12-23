#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "matrixOps.h"

#define FILE_IN_NAME "infile"
#define FILE_OUT_NAME "outfile"
#define MAX_RAND 5

int main(int argc, char* argv[]){

    clock_t startTime,endTime;
    startTime = clock();
    srand((unsigned) time(NULL));
    char pathname[50];
    sprintf(pathname, "../test/%s", FILE_IN_NAME);
    FILE* inFile = fopen(pathname, "r");

    if(inFile == NULL){
        printf("Error opening the file \"%s\"\n", FILE_IN_NAME);
        exit(EXIT_FAILURE);
    }

    int rowsA, colsA, rowsB, colsB;    
    
    fscanf(inFile, "%d %d %d %d", &rowsA, &colsA, &rowsB, &colsB);
    int **matrixA = readMatrixFromFile(inFile, rowsA, colsA);
    int **matrixB = readMatrixFromFile(inFile, rowsB, colsB);
    fclose(inFile);

    int **prodMatrix = multiplyMatrices(matrixA, matrixB, rowsA, colsA, rowsB, colsB);

    if(prodMatrix==NULL)    /*Product was not appliable on that matrices*/
        exit(EXIT_FAILURE);
    
    sprintf(pathname, "../test/%s", FILE_OUT_NAME);
    FILE *outFile = fopen(pathname, "w");
    writeMatrixOnFile(outFile, prodMatrix, rowsA, colsB);
    fclose(outFile);

    free(prodMatrix);
    free(matrixA);
    free(matrixB);
    
    endTime = clock();
    printf("The computation took %.2lf seconds\nOpen file \"outfile\" in test folder to see the result matrix\n", (double) (endTime-startTime)/CLOCKS_PER_SEC);
    return 0;
}
