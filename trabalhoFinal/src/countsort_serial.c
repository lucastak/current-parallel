// countsort_serial.c
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <libppc.h>

void countsort_serial(int *v, long int n) {
    if (n <= 0) return;

    int max = v[0];
    int min = v[0];

    for (long int i = 1; i < n; i++) {
        if (v[i] > max) max = v[i];
        if (v[i] < min) min = v[i];
    }

    int range = max - min + 1;
    int *count = (int*) calloc(range, sizeof(int));
    if (!count) {
        fprintf(stderr, "Erro ao alocar vetor de contagem\n");
        exit(EXIT_FAILURE);
    }

    // conta ocorrências
    for (long int i = 0; i < n; i++) {
        count[v[i] - min]++;
    }

    // reescreve o vetor ordenado
    long int idx = 0;
    for (int value = 0; value < range; value++) {
        while (count[value] > 0) {
            v[idx++] = value + min;
            count[value]--;
        }
    }

    free(count);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <N> <arquivo_vetor>\n", argv[0]);
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    const char *file_vec = argv[2];

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    int *v = load_int_vector(file_vec, n);
    if (!v) {
        fprintf(stderr, "Erro ao carregar vetor do arquivo %s\n", file_vec);
        return EXIT_FAILURE;
    }

    countsort_serial(v, n);

    if (save_int_vector(v, n, "vetor_ordenado.out") != 0) {
        fprintf(stderr, "Erro ao salvar vetor em vetor_ordenado.out\n");
    }

    free(v);
    return EXIT_SUCCESS;
}
