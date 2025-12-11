#include <stdio.h>
#include <stdlib.h>
#include "libppc.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <N> <min> <max> <arquivo_saida>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s 100000 0 100000 vetor_100k.in\n", argv[0]);
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    int min = atoi(argv[2]);
    int max = atoi(argv[3]);
    const char *filename = argv[4];

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    int *v = generate_random_int_vector(n, min, max);
    if (!v) {
        fprintf(stderr, "Erro ao gerar vetor aleatorio\n");
        return EXIT_FAILURE;
    }

    if (save_int_vector(v, n, filename) != 0) {
        fprintf(stderr, "Erro ao salvar vetor em %s\n", filename);
        free(v);
        return EXIT_FAILURE;
    }

    printf("Gerado arquivo de vetor: %s (tamanho = %ld)\n", filename, n);

    free(v);
    return EXIT_SUCCESS;
}
