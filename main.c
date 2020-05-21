#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int vecinos(char* matrix, int i, int j) {
    printf("%c\n", matrix[0]);
}

int main(int argc, char *argv[]) {
    // reading arguments
    int iterations = atoi(argv[1]);
    int matrix_rows = atoi(argv[2]);
    int matrix_cols = atoi(argv[3]);
    char *file_name = argv[4];

    // defining matrix
//    char matrix[matrix_rows][matrix_cols];
//    memset(matrix, 'o', sizeof matrix);
    char* matrix = malloc(matrix_rows * matrix_cols * sizeof(char));

    // printing game initial state
    for (int i = 0; i < matrix_rows; i++) {
        for (int j = 0; j < matrix_cols; j++) {
            matrix[i*matrix_cols+j] = 'o';
        }
    }

    // fulling matrix with initial state from configuration file
    FILE *file_pointer = fopen(file_name, "r");
    char line_of_file[256];

    while (fgets(line_of_file, sizeof(line_of_file), file_pointer)) {
        char *rest;
        int row = (int) strtol(line_of_file, &rest, 10);
        int column = (int) strtol(rest, &rest, 10);

        matrix[row*matrix_cols+column] = '-';
    }

    fclose(file_pointer);

    // printing game initial state
    for (int i = 0; i < matrix_rows; i++) {
        for (int j = 0; j < matrix_cols; j++) {
            printf("%c", matrix[i*matrix_cols+j]);
            if (j == matrix_cols - 1) printf("\n");
        }
    }

//    vecinos(&matrix, 5, 3);

    return 0;
}