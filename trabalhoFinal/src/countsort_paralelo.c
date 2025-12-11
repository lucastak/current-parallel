#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>
#include "libppc.h"

void countsort_parallel(int *v, long int n, int num_threads) {
    if (n <= 0) return;

    int global_max = v[0];
    int global_min = v[0];

    // Cada thread lê posições distintas de v
    // Usamos reduction para encontrar min e max sem região crítica explícita
    // DEPENDÊNCIA ENTRE ITERAÇÕES:
    // Não há dependência de dados entre iterações de i aqui: cada iteração só lê v[i] e atualiza max/min via reduction
    #pragma omp parallel for num_threads(num_threads) reduction(max:global_max) reduction(min:global_min)
    for (long int i = 0; i < n; i++) {
        if (v[i] > global_max) global_max = v[i];
        if (v[i] < global_min) global_min = v[i];
    }

    int range = global_max - global_min + 1;
    int *count = (int*) calloc(range, sizeof(int));
    if (!count) {
        fprintf(stderr, "Erro ao alocar vetor de contagem\n");
        exit(EXIT_FAILURE);
    }

    // Cada thread preenche seu histograma local
    // DEPENDÊNCIA DE DADOS:
    // Não há dependência entre iterações de i: cada thread apenas incrementa local[v[i] - global_min]
    #pragma omp parallel num_threads(num_threads)
    {
        int *local = (int*) calloc(range, sizeof(int));
        if (!local) {
            fprintf(stderr, "Erro ao alocar histograma local\n");
            exit(EXIT_FAILURE);
        }

        #pragma omp for
        for (long int i = 0; i < n; i++) {
            local[v[i] - global_min]++;
        }

        // REGIÃO CRÍTICA:
        // As threads precisam somar seus histogramas locais no vetor global "count"
        // Aqui há potencial condição de corrida, então usamos uma região crítica
        #pragma omp critical
        {
            for (int i = 0; i < range; i++) {
                count[i] += local[i];
            }
        }

        free(local);
    }

    // Aqui reconstruímos o vetor em série.
    // As iterações de value/idx dependem do estado acumulado de "count".
    // Não é paralelizado justamente por causa dessa dependência sequencial.
    long int idx = 0;
    for (int value = 0; value < range; value++) {
        while (count[value] > 0) {
            v[idx++] = value + global_min;
            count[value]--;
        }
    }

    free(count);
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Uso: %s <N> <arquivo_vetor> [num_threads]\n", argv[0]);
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    const char *file_vec = argv[2];
    int num_threads = (argc == 4) ? atoi(argv[3]) : 4;

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    if (num_threads <= 0) num_threads = 4;

    int *v = load_int_vector(file_vec, n);
    if (!v) {
        fprintf(stderr, "Erro ao carregar vetor do arquivo %s\n", file_vec);
        return EXIT_FAILURE;
    }

    double start = omp_get_wtime();
    countsort_parallel(v, n, num_threads);
    double end = omp_get_wtime();

    if (save_int_vector(v, n, "vetor_ordenado.out") != 0) {
        fprintf(stderr, "Erro ao salvar vetor em vetor_ordenado.out\n");
    }

    printf("Threads: %d\n", num_threads);
    printf("Tempo paralelo (s): %f\n", end - start);

    free(v);
    return EXIT_SUCCESS;
}
