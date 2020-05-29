#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

static FILE *outfile;

char *define_matrix(int rows, int cols) {
    char *matrix = malloc(rows * cols * sizeof(char));

    for (int row_index = 0; row_index < rows; row_index++) {
        for (int col_index = 0; col_index < cols; col_index++) {
            matrix[row_index * cols + col_index] = 'o';
        }
    }

    return matrix;
}

void full_matrix_with_initial_state(char *matrix, int matrix_cols, char *initial_state_file_name) {
    FILE *file_pointer = fopen(initial_state_file_name, "r");
    char line_of_file[256];

    while (fgets(line_of_file, sizeof(line_of_file), file_pointer)) {
        char *rest;
        int row = (int) strtol(line_of_file, &rest, 10);
        int column = (int) strtol(rest, &rest, 10);

        matrix[row * matrix_cols + column] = '-';
    }

    fclose(file_pointer);
}

unsigned int neighbors(unsigned char *a, unsigned int i, unsigned int j, unsigned int M, unsigned int N) {
    unsigned int living_neighbors = 0;

    int upPosition = (i - 1) % M;
    int downPosition = (i + 1) % M;
    int leftPosition = (j - 1) % N;
    int rightPosition = (j + 1) % N;

    if (i == 0) {
        upPosition == M - 1;
    }

    if (j == 0) {
        leftPosition == N - 1;
    }

    // vecino de arriba a la izquierda
    if (a[leftPosition * N + upPosition] == '-') living_neighbors++;

    // vecino de arriba
    if (a[i * N + upPosition] == '-') living_neighbors++;

    // vecino de arriba a la derecha
    if (a[rightPosition * N + upPosition] == '-') living_neighbors++;

    // vecino de la izquierda
    if (a[leftPosition * N + j] == '-') living_neighbors++;

    // vecino de la derecha
    if (a[rightPosition * N + j] == '-') living_neighbors++;

    // vecino de abajo a la izquierda
    if (a[leftPosition * N + downPosition] == '-') living_neighbors++;

    // vecino de abajo
    if (a[i * N + downPosition] == '-') living_neighbors++;

    // vecino de abajo a la derecha
    if (a[rightPosition * N + downPosition] == '-') living_neighbors++;

    return living_neighbors;
}


void evolve(char *matrix, int matrix_rows, int matrix_cols) {
    // using auxiliary matrix to save partial updates
    char auxiliary_matrix[matrix_rows][matrix_cols];
    for (int row_index = 0; row_index < matrix_rows; row_index++) {
        for (int col_index = 0; col_index < matrix_cols; col_index++) {
            auxiliary_matrix[row_index][col_index] = matrix[row_index * matrix_cols + col_index];
        }
    }

    for (int row_index = 0; row_index < matrix_rows; row_index++) {
        for (int col_index = 0; col_index < matrix_cols; col_index++) {
            int is_alive = matrix[row_index * matrix_cols + col_index] == '-';
            int living_neighbors = neighbors(matrix, row_index, col_index, matrix_rows, matrix_cols);

            if (living_neighbors < 2 || living_neighbors > 3)
                auxiliary_matrix[row_index][col_index] = 'o';

            if (is_alive && (living_neighbors == 2 || living_neighbors == 3))
                auxiliary_matrix[row_index][col_index] = '-';

            if (!is_alive && living_neighbors == 3)
                auxiliary_matrix[row_index][col_index] = '-';
        }
    }


    // fulling original matrix with auxiliary matrix updated state
    auxiliary_matrix[matrix_rows][matrix_cols];
    for (int row_index = 0; row_index < matrix_rows; row_index++) {
        for (int col_index = 0; col_index < matrix_cols; col_index++) {
            matrix[row_index * matrix_cols + col_index] = auxiliary_matrix[row_index][col_index];
        }
    }
}

void show(char *matrix, int matrix_rows, int matrix_cols) {
    // printing game state
    for (int row_index = 0; row_index < matrix_rows; row_index++) {
        for (int col_index = 0; col_index < matrix_cols; col_index++) {
            printf("%c", matrix[row_index * matrix_cols + col_index]);
            if (col_index == matrix_cols - 1) printf("\n");
        }
    }
    printf("\n");
}

static void
show_example(char *p)
{
    fprintf(stderr, "Example: \n");
    fprintf(stderr,"\t%s 10 20 20\n", p);
}

static void
show_help(char *msg) {
    fprintf(stderr, "Uso:\n");
    fprintf(stderr, "  conway -h\n");
    fprintf(stderr, "  conway -v\n");
    fprintf(stderr, "  conway i M N inputfile [-o outputprefix]\n");
    fprintf(stderr, "Opciones:\n");
    fprintf(stderr, "  -h, --help: Imprime este mensaje.\n");
    fprintf(stderr, "  -v, --version: Da la versión del programa.\n");
    fprintf(stderr, "  -o Prefijo de los archivos de salida.\n");
}

static void
show_version()
{
    fprintf(stderr, "v1.0\n");
}

int main(int argc, char *argv[]) {

    static struct option long_options[] = {
            {"outfile", 1, 0, 'o'},
            {"help", 0, 0, 'h'},
            {"version", 0, 0, 'v'},
            {0, 0, 0, 0}
    };

    int long_index = 0;
    int opt;
    outfile = stdout;

    while ((opt = getopt_long(argc, argv, "o:hv", long_options, &long_index)) != -1) {

        switch(opt) {
            //TODO fix outfile
            case 'o': /* outfile */
                if (strcmp(optarg, "-") != 0) {
                    errno = 0;
                    outfile = fopen(optarg, "w");

                    if (!outfile) {
                        fprintf(stderr, "Unable to open outfile %s: errno %d\n", optarg, errno);
                        exit(2);
                    }
                }
                break;
            case 'h': /* help */
                show_help(argv[0]);
                exit(0);
                break;

            case 'v': /* version */
                show_version();
                exit(0);
                break;

            default:
                fprintf(stderr, "Unrecognized option %c\n", opt );
                show_help(argv[0]);
                exit(1);
        }
    }

    // reading arguments
    int iterations = atoi(argv[1]);
    int matrix_rows = atoi(argv[2]);
    int matrix_cols = atoi(argv[3]);
    char *initial_state_file_name = argv[4];


    char *matrix = define_matrix(matrix_rows, matrix_cols);
    full_matrix_with_initial_state(matrix, matrix_cols, initial_state_file_name);
    show(matrix, matrix_rows, matrix_cols);

    int done_iterations = 1;
    while (done_iterations <= iterations) {
        evolve(matrix, matrix_rows, matrix_cols);
        show(matrix, matrix_rows, matrix_cols);

        done_iterations++;
    }

    return 0;
}