#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

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

unsigned int neighbors2(unsigned char *a, unsigned int i, unsigned int j, unsigned int M, unsigned int N) {
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
    if (a[upPosition * N + leftPosition] == '-') living_neighbors++;

    // vecino de arriba
    if (a[upPosition * N + j] == '-') living_neighbors++;

    // vecino de arriba a la derecha
    if (a[upPosition * N + rightPosition] == '-') living_neighbors++;

    // vecino de la izquierda
    if (a[i * N + leftPosition] == '-') living_neighbors++;

    // vecino de la derecha
    if (a[i * N + rightPosition] == '-') living_neighbors++;

    // vecino de abajo a la izquierda
    if (a[downPosition * N + leftPosition] == '-') living_neighbors++;

    // vecino de abajo
    if (a[downPosition * N + j] == '-') living_neighbors++;

    // vecino de abajo a la derecha
    if (a[downPosition * N + rightPosition] == '-') living_neighbors++;

    return living_neighbors;
}

//extern unsigned int neighbors(unsigned char* a, unsigned int i, unsigned int j, unsigned int M, unsigned int N);

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
show_example(char *p) {
    fprintf(stderr, "Example: \n");
    fprintf(stderr, "\t%s 10 20 20\n", p);
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
    fprintf(stderr, "Ejemplos:\n");
    fprintf(stderr, "  conway 10 20 20 glider -o estado.\n");
    fprintf(stderr, "  Representa 10 iteraciones del Juego de la Vida en una matriz de 20x20,\n");
    fprintf(stderr, "  con un estado inicial tomado del archivo ‘‘glider’’.\n");
    fprintf(stderr, "  Los archivos de salida se llamarán estado_n.pbm.\n");
    fprintf(stderr, "  Si no se da un prefijo para los archivos de salida,\n");
    fprintf(stderr, "  el prefijo será el nombre del archivo de entrada.\n");
}

static void
show_version() {
    fprintf(stderr, "v1.0\n");
}

int get_value(int fil, int col, int matrix_cols, char *matrix) {
    char value = matrix[fil * matrix_cols + col];
    if (value == 'o') {
        return 0;
    } else {
        return 1;
    }
}

#define PIXEL_WIDTH 50
#define PIXEL_HEIGHT 50
#define MIN_ARGUMENTS 5

void generate_pbm(int iterations, int matrix_rows, int matrix_cols, char *matrix, char *file_name) {

    int width = PIXEL_WIDTH;
    int height = PIXEL_HEIGHT;

    char buffer[30];
    snprintf(buffer, sizeof(buffer), "%s_%d.pbm", file_name, iterations);
    printf("Grabando %s\n", buffer);
    //show(matrix, matrix_rows, matrix_cols);

    FILE *outfile = fopen(buffer, "w");
    // Header
    fprintf(outfile, "%s\n", "P1");
    // Width and Height
    fprintf(outfile, "%d %d\n", matrix_cols * PIXEL_WIDTH, matrix_rows * PIXEL_HEIGHT);
    int i, j, pixY, pixX;
    // Writing the image
    for (i = 0; i < matrix_rows; i++) {
        for (pixY = 0; pixY < height; pixY++) {
            for (j = 0; j < matrix_cols; j++) {
                for (pixX = 0; pixX < width; pixX++) {
                    int value = get_value(i, j, matrix_cols, matrix);
                    fprintf(outfile, "%d ", value);
                }
            }
            putc('\n', outfile);
        }
    }
    fclose(outfile);
}

void game_of_life(int iterations, int matrix_rows, int matrix_cols, char *initial_state_file_name, char *file_name) {

    char *matrix = define_matrix(matrix_rows, matrix_cols);
    full_matrix_with_initial_state(matrix, matrix_cols, initial_state_file_name);

    fprintf(stderr, "Leyendo estado inicial...\n");
    generate_pbm(1, matrix_rows, matrix_cols, matrix, file_name);

    int done_iterations = 2;
    while (done_iterations <= iterations) {
        evolve(matrix, matrix_rows, matrix_cols);
        generate_pbm(done_iterations, matrix_rows, matrix_cols, matrix, file_name);

        done_iterations++;
    }
    fprintf(stderr, "Listo.\n");
}

int main(int argc, char *argv[]) {

    static struct option long_options[] = {
            {"outfile", 1, 0, 'o'},
            {"help",    0, 0, 'h'},
            {"version", 0, 0, 'v'},
            {0,         0, 0, 0}
    };

    int long_index = 0;
    int opt;

    while ((opt = getopt_long(argc, argv, "o:hv", long_options, &long_index)) != -1) {

        switch (opt) {
            case 'o': /* outfile */
                if (strcmp(optarg, "-") != 0) {

                    // reading arguments
                    int iterations = atoi(argv[1]);
                    int matrix_rows = atoi(argv[2]);
                    int matrix_cols = atoi(argv[3]);
                    char *initial_state_file_name = argv[4];
                    char *file_name = argv[6];

                    game_of_life(iterations, matrix_rows, matrix_cols, initial_state_file_name, file_name);
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
                fprintf(stderr, "Unrecognized option %c\n", opt);
                show_help(argv[0]);
                exit(1);
        }
    }

    if (argc == MIN_ARGUMENTS) {

        // reading arguments
        int iterations = atoi(argv[1]);
        int matrix_rows = atoi(argv[2]);
        int matrix_cols = atoi(argv[3]);
        char *initial_state_file_name = argv[4];

        game_of_life(iterations, matrix_rows, matrix_cols, initial_state_file_name, initial_state_file_name);

    } else if (argc < MIN_ARGUMENTS) {

        fprintf(stderr, "Error de argumentos.\n");
        show_help(argv[0]);
    }

    return 0;
}