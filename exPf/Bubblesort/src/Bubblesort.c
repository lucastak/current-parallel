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

#define VSIZE 500
#define VSIZE1 0
#define VSIZE2 500
#define nTHREAD 2

// Descomente esta linha abaixo para imprimir valores dos vetores
//#define __DEBUG__

enum implementations_enum {
	TYPE_SERIAL = 1, 
	TYPE_PARALLEL
} ;

double *Bubblesort_serial(const double *v) {
    double *vR = (double*)malloc(sizeof(double) * VSIZE);
    for (long int i = 0; i < VSIZE; i++) {
        vR[i] = v[i];
    }
	//Início da dependência de dados
    for (long int i = 0; i < VSIZE - 1; i++) {
        for (long int j = 0; j < VSIZE - i - 1; j++) {
            if (vR[j] > vR[j + 1]) {
                double tmp = vR[j];
                vR[j] = vR[j + 1];
                vR[j + 1] = tmp;
            }
        }
    }
	//Fim da dependência de dados
    return vR;
}

double *Bubblesort_parallel(const double *v) {
    double *vR = (double*)malloc(sizeof(double) * VSIZE);
    for (long int i = 0; i < VSIZE; i++) {
        vR[i] = v[i];
    }
    for (long int i = 0; i < VSIZE; i++) {
        int start = i % 2;
        #pragma omp parallel for
        for (long int j = start; j < VSIZE - 1; j += 2) {
			// Possível condição de corrida: threads podem trocar elementos adjacentes simultaneamente
            if (vR[j] > vR[j + 1]) {
                double tmp = vR[j];
                vR[j] = vR[j + 1];
                vR[j + 1] = tmp;
            }
        }
    }
    return vR;
}

int main(int argc, char ** argv){

	srand(time(NULL));

	omp_set_num_threads(4);

	double *v, *v_serial, *v_parallel;
	double start, end;

	// Geração ou carregamento do vetor de entrada
	if (access("v.dat", F_OK) != 0) {
		printf("\nGerando vetor aleatório...");
		v = generate_random_double_vector(VSIZE, VSIZE1, VSIZE2);
		save_double_vector(v, VSIZE, "v.dat");
	} else {
		printf("\nCarregando vetor do arquivo...");
		v = load_double_vector("v.dat", VSIZE);
	}

#ifdef __DEBUG__
	print_double_vector(v, 50);
#endif

	// 🔹 Execução SERIAL
	printf("\nExecutando Bubblesort SERIAL...");
	start = omp_get_wtime();
	v_serial = Bubblesort_serial(v);
	end = omp_get_wtime();
	double tempo_serial=end-start;
	printf("\nTempo de execução (serial): %f segundos\n", tempo_serial);
	save_double_vector(v_serial, VSIZE, "vR_1.dat");

	// 🔹 Execução PARALELA
	printf("\nExecutando Bubblesort PARALELO...");
	start = omp_get_wtime();
	v_parallel = Bubblesort_parallel(v);
	end = omp_get_wtime();
	double tempo_paralelo=end-start;
	printf("\nTempo de execução (paralelo): %f segundos\n", tempo_paralelo);
	save_double_vector(v_parallel, VSIZE, "vR_2.dat");

	double speedup=tempo_serial/tempo_paralelo;

	int num_threads=0;
	omp_set_num_threads(nTHREAD);
	#pragma omp parallel
	{
		#pragma omp single
		num_threads=omp_get_num_threads();
	}
	double eficiencia=speedup/num_threads;
	

	//Impressão speedup e eficiencia
	printf("\n====== MÉTRICAS DE DESEMPENHO ======\n");
	printf("Speedup:	%f\n",speedup);
	printf("Eficiencia: %f (com %d threads)\n",eficiencia, num_threads);

	//  Comparação
	printf("\nComparando resultados...");
	int vectors_equal = compare_double_vector_on_files("vR_1.dat", "vR_2.dat");

	if (vectors_equal) {
		printf("\nOK! Serial and parallel outputs are equal!\n");
	} else {
		printf("\nERROR! Files are NOT equal!\n");
	}

	

#ifdef __DEBUG__
	print_double_vector(v_serial, 20);
	print_double_vector(v_parallel, 20);
#endif

	// Liberação de memória
	free(v);
	free(v_serial);
	free(v_parallel);

	return 0;
}