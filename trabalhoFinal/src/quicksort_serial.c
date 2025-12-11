#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "libppc.h"

static void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static int partition(int *v, int low, int high) {
    int pivot = v[high];
    int i = low - 1;

    // Não há dependência entre iterações de j em termos de leitura de v[j],
    // mas as trocas dependem do estado de i e v[i], então isso é mantido serial.
    for (int j = low; j < high; j++) {
        if (v[j] <= pivot) {
            i++;
            swap(&v[i], &v[j]);
        }
    }
    swap(&v[i + 1], &v[high]);
    return i + 1;
}

static void quicksort_serial_impl(int *v, int low, int high) {
    if (low < high) {
        int p = partition(v, low, high);
        quicksort_serial_impl(v, low, p - 1);
        quicksort_serial_impl(v, p + 1, high);
    }
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
    quicksort_serial_impl(v, 0, (int)n - 1);
    double end = omp_get_wtime();

    if (save_int_vector(v, n, file_out) != 0) {
        fprintf(stderr, "Erro ao salvar vetor ordenado em %s\n", file_out);
    }

    printf("Tempo serial (s): %f\n", end - start);

    free(v);
    return EXIT_SUCCESS;
}
