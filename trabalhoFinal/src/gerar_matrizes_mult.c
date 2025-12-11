#include <stdio.h>
#include <stdlib.h>
#include "libppc.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        fprintf(stderr, "Gera matriz1.in e matriz2.in com dimensao N x N.\n");
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    printf("Gerando matrizes %ldx%ld...\n", n, n);

    // matriz1.in
    double *m1 = generate_random_double_matrix(n, n);
    if (!m1) {
        fprintf(stderr, "Erro ao gerar matriz 1\n");
        return EXIT_FAILURE;
    }

    if (save_double_matrix(m1, n, n, "matriz1.in") != 0) {
        fprintf(stderr, "Erro ao salvar matriz1.in\n");
        free(m1);
        return EXIT_FAILURE;
    }

    // matriz2.in
    double *m2 = generate_random_double_matrix(n, n);
    if (!m2) {
        fprintf(stderr, "Erro ao gerar matriz 2\n");
        free(m1);
        return EXIT_FAILURE;
    }

    if (save_double_matrix(m2, n, n, "matriz2.in") != 0) {
        fprintf(stderr, "Erro ao salvar matriz2.in\n");
        free(m1);
        free(m2);
        return EXIT_FAILURE;
    }

    printf("Arquivos gerados: matriz1.in, matriz2.in\n");

    free(m1);
    free(m2);

    return EXIT_SUCCESS;
}
