#include <stdio.h>
#include <stdlib.h>
#include "libppc.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        fprintf(stderr, "Gera matriz3.in representando uma matriz aumentada N x (N+1).\n");
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    long int cols = n + 1;

    printf("Gerando matriz aumentada %ld x %ld em matriz3.in...\n", n, cols);

    double *A = generate_random_double_matrix(n, cols);
    if (!A) {
        fprintf(stderr, "Erro ao gerar matriz aumentada\n");
        return EXIT_FAILURE;
    }

    if (save_double_matrix(A, n, cols, "matriz3.in") != 0) {
        fprintf(stderr, "Erro ao salvar matriz3.in\n");
        free(A);
        return EXIT_FAILURE;
    }

    printf("Arquivo gerado: matriz3.in\n");

    free(A);
    return EXIT_SUCCESS;
}
