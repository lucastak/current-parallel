#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/*
 *
 * Implementação PARALELA da multiplicação de matrizes quadradas usando OpenMP.
 *
 * Uso:
 *   ./matrixmult_paralelo 2 matriz1.in matriz2.in matriz_paralelo.out 4
 *
 * O último parâmetro (num_threads) define quantas threads OpenMP serão usadas.
 */

static double* alloc_matrix(int n) {
    double* m = (double*) malloc(sizeof(double) * n * n);
    if (!m) {
        fprintf(stderr, "Erro ao alocar memoria para matriz %d x %d\n", n, n);
        exit(EXIT_FAILURE);
    }
    return m;
}

static void read_matrix(const char* filename, double* m, int n) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Erro ao abrir arquivo de entrada");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n * n; i++) {
        if (fscanf(f, "%lf", &m[i]) != 1) {
            fprintf(stderr, "Erro ao ler elemento %d do arquivo %s\n", i, filename);
            fclose(f);
            exit(EXIT_FAILURE);
        }
    }
    fclose(f);
}

static void write_matrix(const char* filename, const double* m, int n) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        perror("Erro ao abrir arquivo de saída");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(f, "%.6f", m[i * n + j]);
            if (j < n - 1) {
                fputc(' ', f);
            }
        }
        fputc('\n', f);
    }
    fclose(f);
}

static void matrix_mult_paralelo(const double* A, const double* B, double* C, int n) {
    // Versão paralela: paralelizamos os dois laços externos (i,j)
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        fprintf(stderr,
                "Uso: %s N matriz1.in matriz2.in matriz_out.in num_threads\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    const char* fileA = argv[2];
    const char* fileB = argv[3];
    const char* fileOut = argv[4];
    int num_threads = atoi(argv[5]);

    if (n <= 0 || num_threads <= 0) {
        fprintf(stderr, "N e num_threads devem ser > 0\n");
        return EXIT_FAILURE;
    }

    omp_set_num_threads(num_threads);

    double* A = alloc_matrix(n);
    double* B = alloc_matrix(n);
    double* C = alloc_matrix(n);

    read_matrix(fileA, A, n);
    read_matrix(fileB, B, n);

    double start = omp_get_wtime();
    matrix_mult_paralelo(A, B, C, n);
    double end = omp_get_wtime();

    write_matrix(fileOut, C, n);

    printf("Tempo (paralelo, %d threads) multiplicacao %dx%d: %f segundos\n",
           num_threads, n, n, end - start);

    free(A);
    free(B);
    free(C);
    return EXIT_SUCCESS;
}
