#include <stdio.h>
#include <stdlib.h>
#include <omp.h>      // pode ser usado só para medir tempo, mas não paraleliza aqui
#include "libppc.h"

double* matrix_mult_serial(const double *m1, const double *m2, long int n) {
    double *mR = (double*) malloc(sizeof(double) * n * n);
    if (mR == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para matriz resultado\n");
        exit(EXIT_FAILURE);
    }

    for (long int i = 0; i < n; i++) {
        for (long int j = 0; j < n; j++) {
            M(i, j, n, mR) = 0.0;

            for (long int k = 0; k < n; k++) {
                M(i, j, n, mR) += M(i, k, n, m1) * M(k, j, n, m2);
            }
        }
    }

    return mR;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <N> <arquivo_matriz1> <arquivo_matriz2>\n", argv[0]);
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    const char *file_m1 = argv[2];
    const char *file_m2 = argv[3];

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
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
    double *mR = matrix_mult_serial(m1, m2, n);
    double end = omp_get_wtime();

    // salva resultado
    if (save_double_matrix(mR, n, n, "matriz_mult.out") != 0) {
        fprintf(stderr, "Erro ao salvar matriz resultado em matriz_mult.out\n");
    }

    printf("Tempo serial (s): %f\n", end - start);

    free(m1);
    free(m2);
    free(mR);

    return EXIT_SUCCESS;
}
