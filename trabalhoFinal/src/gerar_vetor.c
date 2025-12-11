#include <stdio.h>
#include <stdlib.h>
#include "libppc.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <N> <min> <max>\n", argv[0]);
        fprintf(stderr, "Gera vetor.in com N inteiros aleatorios no intervalo [min, max].\n");
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    int min = atoi(argv[2]);
    int max = atoi(argv[3]);

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    if (min > max) {
        fprintf(stderr, "min deve ser <= max\n");
        return EXIT_FAILURE;
    }

    printf("Gerando vetor.in com %ld elementos em [%d, %d]...\n", n, min, max);

    int *v = generate_random_int_vector(n, min, max);
    if (!v) {
        fprintf(stderr, "Erro ao gerar vetor aleatorio\n");
        return EXIT_FAILURE;
    }

    if (save_int_vector(v, n, "vetor.in") != 0) {
        fprintf(stderr, "Erro ao salvar vetor.in\n");
        free(v);
        return EXIT_FAILURE;
    }

    printf("Arquivo gerado: vetor.in\n");

    free(v);
    return EXIT_SUCCESS;
}
