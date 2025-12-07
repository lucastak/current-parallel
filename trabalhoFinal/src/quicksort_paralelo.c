#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <libppc.h>

static void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static int partition(int *v, int low, int high) {
    int pivot = v[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (v[j] <= pivot) {
            i++;
            swap(&v[i], &v[j]);
        }
    }
    swap(&v[i + 1], &v[high]);
    return i + 1;
}

static void quicksort_parallel_rec(int *v, int low, int high, int depth) {
    if (low < high) {
        int p = partition(v, low, high);

        if (depth <= 0) {
            if (low < p - 1) quicksort_parallel_rec(v, low, p - 1, depth - 1);
            if (p + 1 < high) quicksort_parallel_rec(v, p + 1, high, depth - 1);
        } else {
            #pragma omp parallel sections
            {
                #pragma omp section
                quicksort_parallel_rec(v, low, p - 1, depth - 1);

                #pragma omp section
                quicksort_parallel_rec(v, p + 1, high, depth - 1);
            }
        }
    }
}

void quicksort_parallel(int *v, long int n, int num_threads) {
    if (n <= 1) return;

    omp_set_num_threads(num_threads);

    int max_depth = 4;
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            quicksort_parallel_rec(v, 0, (int)n - 1, max_depth);
        }
    }
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
    quicksort_parallel(v, n, num_threads);
    double end = omp_get_wtime();

    if (save_int_vector(v, n, "vetor_ordenado.out") != 0) {
        fprintf(stderr, "Erro ao salvar vetor em vetor_ordenado.out\n");
    }

    printf("Threads: %d\n", num_threads);
    printf("Tempo paralelo (s): %f\n", end - start);

    free(v);
    return EXIT_SUCCESS;
}
