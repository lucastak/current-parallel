#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "libppc.h"

int* countsort_parallel_impl(const int *v, long int n, int num_threads) {
    if (n <= 0) return NULL;

    int global_min = v[0];
    int global_max = v[0];

    // DEPENDÊNCIAS DE DADOS:
    // As iterações de i são independentes entre si, e usamos reduction
    // para combinar os resultados de min e max entre as threads.
    #pragma omp parallel for num_threads(num_threads) reduction(min:global_min) reduction(max:global_max)
    for (long int i = 0; i < n; i++) {
        if (v[i] < global_min) global_min = v[i];
        if (v[i] > global_max) global_max = v[i];
    }

    int range = global_max - global_min + 1;
    int *count = (int*) calloc(range, sizeof(int));
    if (!count) {
        fprintf(stderr, "Erro ao alocar vetor de contagem global\n");
        exit(EXIT_FAILURE);
    }

    // Histogramas locais para evitar condição de corrida no vetor count
    #pragma omp parallel num_threads(num_threads)
    {
        int *local = (int*) calloc(range, sizeof(int));
        if (!local) {
            fprintf(stderr, "Erro ao alocar histograma local\n");
            exit(EXIT_FAILURE);
        }

        // Cada thread preenche seu próprio histograma local.
        // Não há dependência de dados entre iterações de i,
        // pois cada thread só escreve em seu vetor local.
        #pragma omp for
        for (long int i = 0; i < n; i++) {
            local[v[i] - global_min]++;
        }

        // REGIÃO CRÍTICA:
        // Aqui, todas as threads atualizam o vetor global count.
        // Para evitar condição de corrida, usamos uma região crítica.
        #pragma omp critical
        {
            for (int i = 0; i < range; i++) {
                count[i] += local[i];
            }
        }

        free(local);
    }

    int *sorted = (int*) malloc(sizeof(int) * n);
    if (!sorted) {
        fprintf(stderr, "Erro ao alocar vetor ordenado\n");
        free(count);
        exit(EXIT_FAILURE);
    }

    // Reconstrução do vetor é sequencial devido à dependência em idx e count[value].
    long int idx = 0;
    for (int value = 0; value < range; value++) {
        while (count[value] > 0) {
            sorted[idx++] = value + global_min;
            count[value]--;
        }
    }

    free(count);
    return sorted;
}

int main(int argc, char *argv[]) {
    // ./countsort_paralelo <N> <arquivo_entrada> <arquivo_saida> [num_threads]
    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Uso: %s <N> <arquivo_entrada> <arquivo_saida> [num_threads]\n", argv[0]);
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    const char *file_in = argv[2];
    const char *file_out = argv[3];
    int num_threads = (argc == 5) ? atoi(argv[4]) : 4;

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    if (num_threads <= 0) {
        num_threads = 4;
    }

    int *v = load_int_vector(file_in, n);
    if (!v) {
        fprintf(stderr, "Erro ao carregar vetor de %s\n", file_in);
        return EXIT_FAILURE;
    }

    double start = omp_get_wtime();
    int *sorted = countsort_parallel_impl(v, n, num_threads);
    double end = omp_get_wtime();

    if (save_int_vector(sorted, n, file_out) != 0) {
        fprintf(stderr, "Erro ao salvar vetor ordenado em %s\n", file_out);
    }

    printf("Threads: %d\n", num_threads);
    printf("Tempo paralelo (s): %f\n", end - start);

    free(v);
    free(sorted);

    return EXIT_SUCCESS;
}
