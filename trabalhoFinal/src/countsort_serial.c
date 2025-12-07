#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/*
 *
 * Implementação SERIAL do algoritmo CountSort para um vetor de doubles.
 *
 * Assumimos que os valores de entrada são inteiros não-negativos armazenados
 * em double (ex.: 0.0, 1.0, 2.0, ...). Se forem valores com casas decimais,
 * será usado o valor inteiro obtido por (int)valor.
 *
 * Uso:
 *   ./countsort_serial 10 vetor.in vetor_count_serial.out
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

static void countsort_serial(double* v, long n) {
    if (n <= 0) return;

    int min = (int)v[0];
    int max = (int)v[0];

    for (long i = 1; i < n; i++) {
        int val = (int)v[i];
        if (val < min) min = val;
        if (val > max) max = val;
    }

    long range = (long)max - (long)min + 1;
    long* count = (long*) calloc(range, sizeof(long));
    if (!count) {
        fprintf(stderr, "Erro ao alocar memoria para vetor de contagem\n");
        exit(EXIT_FAILURE);
    }

    // Conta ocorrências
    for (long i = 0; i < n; i++) {
        int val = (int)v[i];
        count[val - min]++;
    }

    // Reconstrói o vetor ordenado
    long idx = 0;
    for (long r = 0; r < range; r++) {
        while (count[r] > 0) {
            v[idx++] = (double)(r + min);
            count[r]--;
        }
    }

    free(count);
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
    countsort_serial(v, n);
    double end = omp_get_wtime();

    write_vector(fileOut, v, n);

    printf("Tempo (serial) CountSort N=%ld: %f segundos\n", n, end - start);

    free(v);
    return EXIT_SUCCESS;
}
