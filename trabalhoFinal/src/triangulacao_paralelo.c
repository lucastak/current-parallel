#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "libppc.h"

void gaussian_elimination_parallel(double *A, long int n, int num_threads) {
    long int cols = n + 1;

    for (long int k = 0; k < n - 1; k++) {
        double pivot = M(k, k, cols, A);
        if (pivot == 0.0) {
            fprintf(stderr, "Aviso: pivô zero na linha %ld (sem pivotamento)\n", k);
            continue;
        }

        // DEPENDÊNCIA ENTRE ITERAÇÕES DO LOOP k:
        // A iteração k+1 depende da matriz resultante de k,
        // portanto o loop em k permanece sequencial.

        // Porém, para um k fixo, as linhas i > k são independentes entre si.
        // Assim, podemos paralelizar o loop em i.
        #pragma omp parallel for num_threads(num_threads) default(none) shared(A, n, cols, k, pivot)
        for (long int i = k + 1; i < n; i++) {
            double factor = M(i, k, cols, A) / pivot;

            for (long int j = k; j < cols; j++) {
                M(i, j, cols, A) -= factor * M(k, j, cols, A);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // ./triangulacao_paralelo <N> <arquivo_matriz_entrada> <arquivo_saida> [num_threads]
    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Uso: %s <N> <arquivo_matriz_entrada> <arquivo_saida> [num_threads]\n", argv[0]);
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    const char *file_in = argv[2];
    const char *file_out = argv[3];
    int num_threads = (argc == 5) ? atoi(argv[4]) : 4;

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    if (num_threads <= 0) {
        num_threads = 4;
    }

    long int cols = n + 1;
    double *A = load_double_matrix(file_in, n, cols);
    if (!A) {
        fprintf(stderr, "Erro ao carregar matriz de %s\n", file_in);
        return EXIT_FAILURE;
    }

    double start = omp_get_wtime();
    gaussian_elimination_parallel(A, n, num_threads);
    double end = omp_get_wtime();

    if (save_double_matrix(A, n, cols, file_out) != 0) {
        fprintf(stderr, "Erro ao salvar matriz triangulada em %s\n", file_out);
    }

    printf("Threads: %d\n", num_threads);
    printf("Tempo paralelo (s): %f\n", end - start);

    free(A);
    return EXIT_SUCCESS;
}
