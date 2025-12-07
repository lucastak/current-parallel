#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/*
 *
 * A ideia é:
 *  - Criar uma região paralela única com 'omp parallel'
 *  - Criar uma tarefa inicial com 'omp single'
 *  - Dentro da função recursiva, criar novas tasks para cada subproblema,
 *    até uma profundidade máxima para evitar overhead de criação excessiva.
 *
 * Uso:
 *   ./quicksort_paralelo 10 vetor.in vetor_quick_paralelo.out 4
 */

#define MAX_DEPTH 4  /* profundidade máxima de tarefas */

static double* alloc_vector(long n) {
    double* v = (double*) malloc(sizeof(double) * n);
    if (!v) {
        fprintf(stderr, "Erro ao alocar memoria para vetor de tamanho %ld\n", n);
        exit(EXIT_FAILURE);
    }
    return v;
}

static void read_vector(const char* filename, double* v, long n) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Erro ao abrir arquivo de entrada");
        exit(EXIT_FAILURE);
    }
    for (long i = 0; i < n; i++) {
        if (fscanf(f, "%lf", &v[i]) != 1) {
            fprintf(stderr, "Erro ao ler elemento %ld do arquivo %s\n", i, filename);
            fclose(f);
            exit(EXIT_FAILURE);
        }
    }
    fclose(f);
}

static void write_vector(const char* filename, const double* v, long n) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        perror("Erro ao abrir arquivo de saída");
        exit(EXIT_FAILURE);
    }
    for (long i = 0; i < n; i++) {
        fprintf(f, "%.6f\n", v[i]);
    }
    fclose(f);
}

static void swap(double* a, double* b) {
    double tmp = *a;
    *a = *b;
    *b = tmp;
}

static long partition(double* v, long low, long high) {
    double pivot = v[high];
    long i = low - 1;
    for (long j = low; j < high; j++) {
        if (v[j] <= pivot) {
            i++;
            swap(&v[i], &v[j]);
        }
    }
    swap(&v[i + 1], &v[high]);
    return i + 1;
}

/* Versão recursiva usada tanto na serial quanto na paralela */
static void quicksort_serial_rec(double* v, long low, long high) {
    if (low < high) {
        long p = partition(v, low, high);
        quicksort_serial_rec(v, low, p - 1);
        quicksort_serial_rec(v, p + 1, high);
    }
}

/* Função recursiva paralela com tasks */
static void quicksort_paralelo_rec(double* v, long low, long high, int depth) {
    if (low < high) {
        long p = partition(v, low, high);

        if (depth < MAX_DEPTH) {
            #pragma omp task firstprivate(v, low, p, depth)
            quicksort_paralelo_rec(v, low, p - 1, depth + 1);

            #pragma omp task firstprivate(v, high, p, depth)
            quicksort_paralelo_rec(v, p + 1, high, depth + 1);
        } else {
            quicksort_serial_rec(v, low, p - 1);
            quicksort_serial_rec(v, p + 1, high);
        }
    }
}

static void quicksort_paralelo(double* v, long n, int num_threads) {
    if (n <= 1) return;

    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
        #pragma omp single nowait
        quicksort_paralelo_rec(v, 0, n - 1, 0);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr,
                "Uso: %s N vetor.in vetor_ordenado.out num_threads\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    long n = atol(argv[1]);
    const char* fileIn = argv[2];
    const char* fileOut = argv[3];
    int num_threads = atoi(argv[4]);

    if (n <= 0 || num_threads <= 0) {
        fprintf(stderr, "N e num_threads devem ser > 0\n");
        return EXIT_FAILURE;
    }

    double* v = alloc_vector(n);
    read_vector(fileIn, v, n);

    double start = omp_get_wtime();
    quicksort_paralelo(v, n, num_threads);
    double end = omp_get_wtime();

    write_vector(fileOut, v, n);

    printf("Tempo (paralelo, %d threads) QuickSort N=%ld: %f segundos\n",
           num_threads, n, end - start);

    free(v);
    return EXIT_SUCCESS;
}
