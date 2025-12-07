#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include <libppc.h>

#include <omp.h>

#define VSIZE 100000
#define VSIZE1 0
#define VSIZE2 100000
#define nTHREAD 2

// Descomente esta linha abaixo para imprimir valores dos vetores
//#define __DEBUG__

enum implementations_enum {
    TYPE_SERIAL = 1,
    TYPE_PARALLEL
};

// Função auxiliar: intercala dois subvetores
// Região de dependência de dados: leitura e escrita em posições sobrepostas de 'v' e 'temp'
// Esta função não é paralelizada, mas deve ser chamada com segurança para evitar condição de corrida
void merge(double *v, long int left, long int mid, long int right, double *temp) {
    long int i = left, j = mid, k = left;
    while (i < mid && j < right) {
        if (v[i] <= v[j])
            temp[k++] = v[i++];
        else
            temp[k++] = v[j++];
    }
    while (i < mid) temp[k++] = v[i++];
    while (j < right) temp[k++] = v[j++];
    // Região crítica implícita: sobrescreve o vetor original 'v' com dados de 'temp'
    for (i = left; i < right; i++) v[i] = temp[i];
}

void merge_sort_serial_rec(double *v, long int left, long int right, double *temp) {
    if (right - left > 1) {
        long int mid = (left + right) / 2;
        merge_sort_serial_rec(v, left, mid, temp);
        merge_sort_serial_rec(v, mid, right, temp);
        merge(v, left, mid, right, temp);
    }
}

// Início da região de dependência: alocação e cópia de dados
// O vetor original 'v' é copiado para 'vR', que será ordenado
double *MergeSort_serial(const double *v) {
    double *vR = (double *)malloc(sizeof(double) * VSIZE);
    double *temp = (double *)malloc(sizeof(double) * VSIZE);

    // Região de dependência: leitura do vetor de entrada 'v' e escrita em 'vR'
    for (long int i = 0; i < VSIZE; i++) vR[i] = v[i];

    // Execução serial do algoritmo de ordenação
    merge_sort_serial_rec(vR, 0, VSIZE, temp);

    // Fim da região de dependência
    free(temp);
    return vR;
}

void merge_sort_parallel_rec(double *v, long int left, long int right, double *temp, int depth) {
    if (right - left > 1) {
        long int mid = (left + right) / 2;
        if (depth <= 0) {
            // Região sequencial: quando profundidade termina, executa recursão serial
            merge_sort_serial_rec(v, left, mid, temp);
            merge_sort_serial_rec(v, mid, right, temp);
        } else {
            // Início da região crítica paralela: acesso concorrente a diferentes segmentos do vetor
            // Cada seção trabalha com uma parte distinta e independente do vetor
            #pragma omp parallel sections
            {
                #pragma omp section
                merge_sort_parallel_rec(v, left, mid, temp, depth - 1);
                #pragma omp section
                merge_sort_parallel_rec(v, mid, right, temp, depth - 1);
            }
            // Fim da região paralela
        }

        // Região crítica: chamada a 'merge' combina dados das duas seções ordenadas
        // Deve ocorrer fora do bloco paralelo para evitar condição de corrida
        merge(v, left, mid, right, temp);
    }
}

// Início da região de dependência: alocação e cópia do vetor de entrada
// Cada thread terá acesso ao vetor completo, mas as operações são divididas por segmentos
double *MergeSort_parallel(const double *v) {
    double *vR = (double *)malloc(sizeof(double) * VSIZE);
    double *temp = (double *)malloc(sizeof(double) * VSIZE);

    // Região de dependência: leitura sequencial e inicialização do vetor de trabalho
    for (long int i = 0; i < VSIZE; i++) vR[i] = v[i];

    // Chamada à função paralela que contém regiões críticas e dependência de dados
    merge_sort_parallel_rec(vR, 0, VSIZE, temp, 4);
    //Fim de região de dependência   
    free(temp);
    return vR;
}

int main(int argc, char **argv) {
    srand(time(NULL));

    double *v, *v_serial, *v_parallel;
    double start, end, tempo_serial, tempo_paralelo;

    if (access("v.dat", F_OK) != 0) {
        printf("\nGerando vetor aleatório...");
        v = generate_random_double_vector(VSIZE, VSIZE1, VSIZE2);
        save_double_vector(v, VSIZE, "v.dat");
    } else {
        printf("\nCarregando vetor do arquivo...");
        v = load_double_vector("v.dat", VSIZE);
    }

#ifdef __DEBUG__
    print_double_vector(v, VSIZE);
#endif

    printf("\nExecutando MergeSort serial...");
    start = omp_get_wtime();
    v_serial = MergeSort_serial(v);
    end = omp_get_wtime();
    tempo_serial = end - start;
    printf("\nTempo de execução (serial): %f segundos\n", tempo_serial);
    save_double_vector(v_serial, VSIZE, "vR_1.dat");

    printf("\nExecutando MergeSort paralelo...");
    start = omp_get_wtime();
    v_parallel = MergeSort_parallel(v);
    end = omp_get_wtime();
    tempo_paralelo = end - start;
    printf("\nTempo de execução (paralelo): %f segundos\n", tempo_paralelo);
    save_double_vector(v_parallel, VSIZE, "vR_2.dat");

    printf("\nComparando resultados...");
    int vectors_equal = compare_double_vector_on_files("vR_1.dat", "vR_2.dat");
    if (vectors_equal) {
        printf("\nOK! Serial and parallel outputs are equal!\n");
    } else {
        printf("\nERROR! Files are NOT equal!\n");
    }

    double speedup = tempo_serial / tempo_paralelo;
    int num_threads=0;
    omp_set_num_threads(nTHREAD);
    #pragma omp parallel
    {
        #pragma omp master
        num_threads = omp_get_num_threads();
    }
    double eficiencia = speedup / num_threads;

    printf("\nSpeedup: %.2f\n", speedup);
    printf("Eficiência: %.2f (com %d threads)\n)", eficiencia,num_threads);

#ifdef __DEBUG__
    print_double_vector(v_serial, 100);
    print_double_vector(v_parallel, 100);
#endif

    free(v);
    free(v_serial);
    free(v_parallel);

    return 0;
}
