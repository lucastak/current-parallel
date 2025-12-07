#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/*
 * quicksort_serial.c
 *
 * Implementação SERIAL do algoritmo QuickSort para vetor de doubles.
 *
 * Uso:
 *   ./quicksort_serial N vetor.in vetor_ordenado.out
 */

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

static void quicksort_serial_rec(double* v, long low, long high) {
    if (low < high) {
        long p = partition(v, low, high);
        quicksort_serial_rec(v, low, p - 1);
        quicksort_serial_rec(v, p + 1, high);
    }
}

static void quicksort_serial(double* v, long n) {
    if (n > 1) {
        quicksort_serial_rec(v, 0, n - 1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr,
                "Uso: %s N vetor.in vetor_ordenado.out\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    long n = atol(argv[1]);
    const char* fileIn = argv[2];
    const char* fileOut = argv[3];

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    double* v = alloc_vector(n);
    read_vector(fileIn, v, n);

    double start = omp_get_wtime();
    quicksort_serial(v, n);
    double end = omp_get_wtime();

    write_vector(fileOut, v, n);

    printf("Tempo (serial) QuickSort N=%ld: %f segundos\n", n, end - start);

    free(v);
    return EXIT_SUCCESS;
}
