#include <stdio.h>
#include <stdlib.h>
#include "libppc.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <N> <prefixo>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s 500 matriz\n", argv[0]);
        fprintf(stderr, "Vai gerar: matriz1_500.in e matriz2_500.in\n");
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    const char *prefix = argv[2];

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    double *m1 = generate_random_double_matrix(n, n);
    double *m2 = generate_random_double_matrix(n, n);

    char file1[256];
    char file2[256];

    snprintf(file1, sizeof(file1), "%s1_%ld.in", prefix, n);
    snprintf(file2, sizeof(file2), "%s2_%ld.in", prefix, n);

    save_double_matrix(m1, n, n, file1);
    save_double_matrix(m2, n, n, file2);

    printf("Gerados arquivos: %s e %s\n", file1, file2);

    free(m1);
    free(m2);
    return EXIT_SUCCESS;
}
