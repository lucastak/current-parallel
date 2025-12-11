#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "libppc.h"

int* countsort_serial_impl(const int *v, long int n, int *out_min, int *out_max) {
    if (n <= 0) return NULL;

    int min = v[0];
    int max = v[0];

    // Não há dependência entre iterações de i: cada iteração lê v[i] independentemente
    for (long int i = 0; i < n; i++) {
        if (v[i] < min) min = v[i];
        if (v[i] > max) max = v[i];
    }

    int range = max - min + 1;
    int *count = (int*) calloc(range, sizeof(int));
    if (!count) {
        fprintf(stderr, "Erro ao alocar vetor de contagem\n");
        exit(EXIT_FAILURE);
    }

    for (long int i = 0; i < n; i++) {
        count[v[i] - min]++;
    }

    int *sorted = (int*) malloc(sizeof(int) * n);
    if (!sorted) {
        fprintf(stderr, "Erro ao alocar vetor ordenado\n");
        free(count);
        exit(EXIT_FAILURE);
    }

    long int idx = 0;
    for (int value = 0; value < range; value++) {
        while (count[value] > 0) {
            sorted[idx++] = value + min;
            count[value]--;
        }
    }

    free(count);

    if (out_min) *out_min = min;
    if (out_max) *out_max = max;

    return sorted;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <N> <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    const char *file_in = argv[2];
    const char *file_out = argv[3];

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    int *v = load_int_vector(file_in, n);
    if (!v) {
        fprintf(stderr, "Erro ao carregar vetor de %s\n", file_in);
        return EXIT_FAILURE;
    }

    double start = omp_get_wtime();
    int min_val = 0, max_val = 0;
    int *sorted = countsort_serial_impl(v, n, &min_val, &max_val);
    double end = omp_get_wtime();

    if (save_int_vector(sorted, n, file_out) != 0) {
        fprintf(stderr, "Erro ao salvar vetor ordenado em %s\n", file_out);
    }

    printf("Tempo serial (s): %f\n", end - start);

    free(v);
    free(sorted);

    return EXIT_SUCCESS;
}
