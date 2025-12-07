#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/*
 *
 * Implementação SERIAL de eliminação gaussiana (triangulação) para
 * uma matriz aumentada N x (N+1). A matriz de entrada representa o
 * sistema linear A.x = b.
 *
 * Uso:
 *   ./triangulacao_paralelo 2 matriz3.in saida_paralelo.out
 *
 * Formato esperado do arquivo de entrada (matriz.in):
 *   N linhas, cada linha com (N+1) valores double.
 */

static double* alloc_matrix(int n) {
    double* A = (double*) malloc(sizeof(double) * n * (n + 1));
    if (!A) {
        fprintf(stderr, "Erro ao alocar memoria para matriz aumentada\n");
        exit(EXIT_FAILURE);
    }
    return A;
}

static void read_augmented_matrix(const char* filename, double* A, int n) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Erro ao abrir arquivo de entrada");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n + 1; j++) {
            if (fscanf(f, "%lf", &A[i * (n + 1) + j]) != 1) {
                fprintf(stderr, "Erro ao ler matriz na linha %d coluna %d\n", i, j);
                fclose(f);
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(f);
}

static void write_augmented_matrix(const char* filename, const double* A, int n) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        perror("Erro ao abrir arquivo de saída");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n + 1; j++) {
            fprintf(f, "%.6f", A[i * (n + 1) + j]);
            if (j < n) fputc(' ', f);
        }
        fputc('\n', f);
    }
    fclose(f);
}

static void gaussian_elimination_serial(double* A, int n) {
    for (int k = 0; k < n - 1; k++) {
        double pivot = A[k * (n + 1) + k];
        if (pivot == 0.0) {
            fprintf(stderr, "Pivo zero na linha %d, coluna %d (sem pivoteamento)\n", k, k);
            continue;
        }

        for (int i = k + 1; i < n; i++) {
            double factor = A[i * (n + 1) + k] / pivot;
            for (int j = k; j < n + 1; j++) {
                A[i * (n + 1) + j] -= factor * A[k * (n + 1) + j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr,
                "Uso: %s N matriz.in saida.out\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    const char* fileIn = argv[2];
    const char* fileOut = argv[3];

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    double* A = alloc_matrix(n);
    read_augmented_matrix(fileIn, A, n);

    double start = omp_get_wtime();
    gaussian_elimination_serial(A, n);
    double end = omp_get_wtime();

    write_augmented_matrix(fileOut, A, n);

    printf("Tempo (serial) triangulacao N=%d: %f segundos\n", n, end - start);

    free(A);
    return EXIT_SUCCESS;
}
