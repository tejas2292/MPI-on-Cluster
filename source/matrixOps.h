#ifndef MATRIX_OPS
#define MATRIX_OPS
#include <stdio.h>
#include <stdlib.h>

void writeMatrixOnFile(FILE* file, int **matrix, int rows, int cols);
void writeRandMatrixOnFile(FILE *file, int rows, int cols, int maxRand);
int **readMatrixFromFile(FILE *file, int rows, int cols);
int **allocateMatrix(int rows, int cols); //Allocate memory for a matrix rows*cols
void initializeRandMatrix(int **matrix, int rows, int cols, int maxRand); //Fill a pre-allocated matrix with random values
int **instantiateRandMatrix(int rows, int cols, int maxRand); //Comprises both allocateMatrix and initializeRandMatrix functions
void printMatrix(int **matrix, int rows, int cols);
int **multiplyMatrices(int **matrixA, int **matrixB, int rowsA, int colsA, int rowsB, int colsB);
int *multiplyMatricesretVect(int **matrixA, int **matrixB, int rowsA, int colsA, int rowsB, int colsB, int offset,int elements);

#endif