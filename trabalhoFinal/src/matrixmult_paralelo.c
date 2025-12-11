#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "libppc.h"

double* matrix_mult_parallel(const double *m1, const double *m2, long int n, int num_threads) {
    double *mR = (double*) malloc(sizeof(double) * n * n);
    if (mR == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para matriz resultado\n");
        exit(EXIT_FAILURE);
    }

    // Não há região crítica: cada thread escreve em índices diferentes
    // Cada thread calcula elementos (i, j) independentes da matriz resultado
    // Não há dependência de dados entre iterações (i,j), pois cada posição de mR é escrita uma única vez
    // Assim, podemos paralelizar o laço externo com collapse(2)
    #pragma omp parallel for num_threads(num_threads) collapse(2)
    for (long int i = 0; i < n; i++) {
        for (long int j = 0; j < n; j++) {

            // DEPENDÊNCIA DE DADOS:
            // Dentro de cada iteração (i,j), o acumulador "soma" depende das iterações de k,
            // mas isso é uma dependência local à thread, não entre threads
            // Entre threads (i,j diferentes), não há dependência
            double soma = 0.0;
            for (long int k = 0; k < n; k++) {
                soma += M(i, k, n, m1) * M(k, j, n, m2);
            }
            M(i, j, n, mR) = soma;
        }
    }

    return mR;
}

int main(int argc, char *argv[]) {
    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Uso: %s <N> <arquivo_matriz1> <arquivo_matriz2> [num_threads]\n", argv[0]);
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    const char *file_m1 = argv[2];
    const char *file_m2 = argv[3];
    int num_threads = (argc == 5) ? atoi(argv[4]) : 4;

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    if (num_threads <= 0) {
        num_threads = 4;
    }

    double *m1 = load_double_matrix(file_m1, n, n);
    if (m1 == NULL) {
        fprintf(stderr, "Erro ao carregar matriz 1 do arquivo %s\n", file_m1);
        return EXIT_FAILURE;
    }

    double *m2 = load_double_matrix(file_m2, n, n);
    if (m2 == NULL) {
        fprintf(stderr, "Erro ao carregar matriz 2 do arquivo %s\n", file_m2);
        free(m1);
        return EXIT_FAILURE;
    }

    double start = omp_get_wtime();
    double *mR = matrix_mult_parallel(m1, m2, n, num_threads);
    double end = omp_get_wtime();

    if (save_double_matrix(mR, n, n, "matriz_mult.out") != 0) {
        fprintf(stderr, "Erro ao salvar matriz resultado em matriz_mult.out\n");
    }

    printf("Threads: %d\n", num_threads);
    printf("Tempo paralelo (s): %f\n", end - start);

    free(m1);
    free(m2);
    free(mR);

    return EXIT_SUCCESS;
}
