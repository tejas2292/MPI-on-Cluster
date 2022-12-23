#include "matrixOps.h"

void writeMatrixOnFile(FILE* file, int** matrix, int rows, int cols){
    fprintf(file, "%d %d\n", rows, cols);
    for(int i = 0; i < rows; i++){
        for(int j=0; j < cols; j++){
            fprintf(file, "%d ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }
}

void writeRandMatrixOnFile(FILE* file, int rows, int cols, int maxRand){
    for(int i = 0; i < rows; i++){
            for(int j = 0; j< cols; j++)
                fprintf(file, "%d ", rand() % maxRand);
            fprintf(file, "\n");
    }
}

int **readMatrixFromFile(FILE* file, int rows, int cols){

    int **matrix = allocateMatrix(rows, cols);
    
    for(int i = 0; i < rows; i++){
         for(int j = 0; j < cols; j++)
            fscanf(file, "%d", &matrix[i][j]);
    }

    return matrix;
}

int **allocateMatrix(int rows, int cols){

    int** matrix = malloc(rows * sizeof *matrix);
    
    for(int i = 0; i < rows; i++){
        *(matrix + i) = malloc(cols * sizeof *matrix[i]);
    }
    return matrix;
};


void initializeRandMatrix(int** matrix, int rows, int cols, int maxRand){
    for(int row = 0; row < rows; row++){
        for(int col = 0; col < cols; col++){
            matrix[row][col] = rand() % maxRand;
        }
    }
}

int **instantiateRandMatrix(int rows, int cols, int maxRand){//Using both allocation and initialization methods
    int **matrix = allocateMatrix(rows, cols);
    initializeRandMatrix(matrix, rows, cols, maxRand);

    return matrix;
}

void printMatrix(int** matrix, int rows, int cols){
    printf("Printing matrix\n");
    for(int row = 0; row < rows; row++){
        for(int col = 0; col < cols; col++){
            printf("%d|", matrix[row][col]);
        }
        printf("\n");
    }
}

int** multiplyMatrices(int** matrixA, int** matrixB, int rowsA, int colsA, int rowsB, int colsB){
    if(colsA!=rowsB){
        printf("Product not appliable on these matrixes, please be sure that matrixes are of the form A:mxn B:nxl\n");
        return NULL;
    }
    int** prod_matrix = allocateMatrix(rowsA, colsB);
    int product = 0;

    for(int row_a = 0; row_a < rowsA; row_a++){
        for(int col_a = 0, col_b = 0; col_b < colsB; col_a++){
            
            product += (matrixA[row_a][col_a] * matrixB[col_a][col_b]); 
            
            if(((col_a + 1) == colsA)){
                prod_matrix[row_a][col_b] = product;
                col_a = -1;
                col_b++;
                product = 0;
            }  
        }
    }
    return prod_matrix;
}

int* multiplyMatricesretVect(int** matrixA, int** matrixB, int rowsA, int colsA, int rowsB, int colsB, int offset,int elements){
    if(colsA!=rowsB){
        printf("Product not appliable on these matrixes, please be sure that matrixes are of the form A:mxn B:nxl\n");
        return NULL;
    }
    int* prod_matrix = malloc(sizeof(int) * elements);
    int product = 0;
    int col_b;
    int count = 0;

    for(int row_a = 0; row_a < rowsA; row_a++){
        if(row_a == 0)
            col_b = offset;
        else
            col_b = 0;
        
        for(int col_a = 0; col_b < colsB; col_a++){
            
            product += (matrixA[row_a][col_a] * matrixB[col_a][col_b]); 

            if(((col_a + 1) == colsA)){
                prod_matrix[count++] = product;
                col_a = -1;
                col_b++;
                product = 0;
            }  
            if(count == elements){
                return prod_matrix;
            }
        }

    }
    return prod_matrix;
}

