#include <stdio.h>
#include <stdlib.h>
#include "libppc.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <N> <arquivo_saida>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s 500 matriz_tri_500.in\n", argv[0]);
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    const char *filename = argv[2];

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    double *A = generate_random_double_matrix(n, n);
    if (!A) {
        fprintf(stderr, "Erro ao gerar matriz\n");
        return EXIT_FAILURE;
    }

    if (save_double_matrix(A, n, n, filename) != 0) {
        fprintf(stderr, "Erro ao salvar matriz em %s\n", filename);
        free(A);
        return EXIT_FAILURE;
    }

    printf("Gerado arquivo de matriz: %s (%ld x %ld)\n", filename, n, n);

    free(A);
    return EXIT_SUCCESS;
}
