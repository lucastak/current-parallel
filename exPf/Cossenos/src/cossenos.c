
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <libppc.h>

#include <omp.h>

#define VSIZE 1000
#define VSIZE1 0
#define VSIZE2 1000
#define PI 3.14159265358979323846
#define nTHREAD 2

//#define __DEBUG__

enum implementations_enum {
    TYPE_SERIAL = 1,
    TYPE_PARALLEL
};

// Transformada Discreta do Cosseno - Versão Serial
double *DCT1D_serial(const double *v) {
    double *X = (double *)malloc(sizeof(double) * VSIZE);
    for (long int k = 0; k < VSIZE; k++) {
        X[k] = 0.0;
        for (long int n = 0; n < VSIZE; n++) {
            X[k] += v[n] * cos(PI * (n + 0.5) * k / VSIZE);
        }
    }
    return X;
}

// Transformada Discreta do Cosseno - Versão Paralela
double *DCT1D_parallel(const double *v) {
    double *X = (double *)malloc(sizeof(double) * VSIZE);

    #pragma omp parallel for
    for (long int k = 0; k < VSIZE; k++) {
        X[k] = 0.0;
        for (long int n = 0; n < VSIZE; n++) {
            X[k] += v[n] * cos(PI * (n + 0.5) * k / VSIZE);
        }
    }

    return X;
}

int main(int argc, char **argv) {
   srand(time(NULL));

    double *v, *v_serial, *v_parallel;
    double tempo_serial = 0.0, tempo_paralelo = 0.0;
    double start, end;

    // Geração ou carregamento do vetor
    if (access("v.dat", F_OK) != 0) {
        printf("\nGerando vetor aleatório...");
        v = generate_random_double_vector(VSIZE, VSIZE1, VSIZE2);
        save_double_vector(v, VSIZE, "v.dat");
    } else {
        printf("\nCarregando vetor do arquivo...");
        v = load_double_vector("v.dat", VSIZE);
    }

    // Executa versão SERIAL
    printf("\nExecutando DCT 1D serial...");
    start = omp_get_wtime();
    v_serial = DCT1D_serial(v);
    end = omp_get_wtime();
    tempo_serial = end - start;
    printf("Tempo de execução (serial): %.6f segundos\n", tempo_serial);
    save_double_vector(v_serial, VSIZE, "vR_1.dat");

    // Executa versão PARALELA
    omp_set_num_threads(nTHREAD);
    printf("\nExecutando DCT 1D paralelo...");
    start = omp_get_wtime();
    v_parallel = DCT1D_parallel(v);
    end = omp_get_wtime();
    tempo_paralelo = end - start;
    printf("Tempo de execução (paralelo): %.6f segundos\n", tempo_paralelo);
    save_double_vector(v_parallel, VSIZE, "vR_2.dat");

    // Comparação dos resultados
    printf("\nComparando resultados...");
    int iguais = compare_double_vector_on_files("vR_1.dat", "vR_2.dat");
    if (iguais) {
        printf("\nOK! Serial and parallel outputs are equal!\n");
    } else {
        printf("\nERROR! Files are NOT equal!\n");
    }

    // Métricas de desempenho
    int num_threads = nTHREAD;
    #pragma omp parallel
    {
        #pragma omp master
        num_threads = omp_get_num_threads();
    }

    double speedup = tempo_serial / tempo_paralelo;
    double eficiencia = speedup / num_threads;

    printf("\n====== MÉTRICAS DE DESEMPENHO ======\n");
    printf("Speedup:     %.2f\n", speedup);
    printf("Eficiência:  %.2f (com %d threads)\n", eficiencia, num_threads);

    free(v);
    free(v_serial);
    free(v_parallel);
    return 0;
}