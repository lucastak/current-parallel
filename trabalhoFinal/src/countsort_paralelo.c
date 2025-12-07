#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/*
 *
 * Implementação PARALELA do algoritmo CountSort usando OpenMP.
 *
 * A principal região paralelizável é a etapa de contagem de ocorrências.
 * Usamos diretiva 'omp parallel for' com 'atomic' para evitar condições de corrida
 * ao atualizar o vetor de contagem.
 *
 * Uso:
 *   ./countsort_paralelo 10 vetor.in vetor_count_paralelo.out 4
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

static void countsort_paralelo(double* v, long n, int num_threads) {
    if (n <= 0) return;

    int min = (int)v[0];
    int max = (int)v[0];

    // Encontrar min e max (aqui fazemos em série por simplicidade)
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

    omp_set_num_threads(num_threads);

    // Etapa de contagem paralela
    #pragma omp parallel for
    for (long i = 0; i < n; i++) {
        int val = (int)v[i];
        long idx = (long)val - (long)min;
        #pragma omp atomic
        count[idx]++;
    }

    // Reconstrói o vetor ordenado (em série)
    long k = 0;
    for (long r = 0; r < range; r++) {
        while (count[r] > 0) {
            v[k++] = (double)(r + min);
            count[r]--;
        }
    }

    free(count);
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
    countsort_paralelo(v, n, num_threads);
    double end = omp_get_wtime();

    write_vector(fileOut, v, n);

    printf("Tempo (paralelo, %d threads) CountSort N=%ld: %f segundos\n",
           num_threads, n, end - start);

    free(v);
    return EXIT_SUCCESS;
}
