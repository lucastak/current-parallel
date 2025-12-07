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

#define NLINES 1000
#define NCOLS 1000
#define nTHREAD 1

// Descomente esta linha abaixo para imprimir valores das matrizes 
//#define __DEBUG__

enum implementations_enum {
	TYPE_SERIAL = 1, 
	TYPE_PARALLEL
} ;

// Função serial com dependência de dados (acúmulo na célula mR[i][j])
double *MatrixMult_serial(const double *m1, const double *m2){

	double *mR = (double*)malloc(
		sizeof(double) * NLINES * NCOLS);

	for ( long int i = 0; i < NLINES; i++ ){

		for ( long int j = 0; j < NCOLS; j++ ){

			// Utilize esta macro abaixo para acessar valores de matrizes em C
			M( i , j , NCOLS, mR) = 0;
            //Início da dependência de dados
			for (long int k = 0; k < NCOLS; k++ ){

				M(i, j, NCOLS, mR) += 
					M( i , k , NCOLS, m1) * M( k , j , NCOLS, m2);

			}
            //Fim da dependência de dados
		}
	}
	
	return mR;
}


// Função paralela com paralelismo nas dimensões i e j
// Não há região crítica pois cada thread trabalha em células exclusivas mR[i][j]
double *MatrixMult_parallel(const double *m1, const double *m2){

	double *mR = (double*)malloc(
		sizeof(double) * NLINES * NCOLS);

    // Início da região paralela
	#pragma omp parallel for collapse(2)
	for (long int i = 0; i < NLINES; i++) {
    	for (long int j = 0; j < NCOLS; j++) {
        	M(i, j, NCOLS, mR) = 0;
        	for (long int k = 0; k < NCOLS; k++) {
            	M(i, j, NCOLS, mR) += 
                	M(i, k, NCOLS, m1) * M(k, j, NCOLS, m2);
        	}
    	}
	}
	// Fim da região paralela
	return mR;
}


int main(int argc, char ** argv){
	srand(time(NULL));
    double *m1, *m2, *mR_serial, *mR_parallel;
    double start, end, time_serial, time_parallel;

    if (access("m1.dat", F_OK) != 0) {
        printf("\nGenerating new Matrix 1 values...");
        m1 = (double*)generate_random_double_matrix(NLINES, NCOLS);
        save_double_matrix(m1, NLINES, NCOLS, "m1.dat");
    } else {
        printf("\nLoading Matrix 1 from file ...");
        m1 = load_double_matrix("m1.dat", NLINES, NCOLS);
    }

    if (access("m2.dat", F_OK) != 0) {
        printf("\nGenerating new Matrix 2 values...");
        m2 = (double*)generate_random_double_matrix(NLINES, NCOLS);
        save_double_matrix(m2, NLINES, NCOLS, "m2.dat");
    } else {
        printf("\nLoading Matrix 2 from file ...");
        m2 = load_double_matrix("m2.dat", NLINES, NCOLS);
    }

    // Execução SERIAL
    printf("\nRunning serial implementation ...");
    start = omp_get_wtime();
    mR_serial = MatrixMult_serial(m1, m2);
    end = omp_get_wtime();
    time_serial = end - start;
    printf("\nTempo de execução (serial): %f segundos\n", time_serial);
    save_double_matrix(mR_serial, NLINES, NCOLS, "mR_1.dat");

    // Execução PARALELA
    printf("\nRunning parallel implementation ...");
    start = omp_get_wtime();
    mR_parallel = MatrixMult_parallel(m1, m2);
    end = omp_get_wtime();
    time_parallel = end - start;
    printf("\nTempo de execução (paralelo): %f segundos\n", time_parallel);
    save_double_matrix(mR_parallel, NLINES, NCOLS, "mR_2.dat");

    // Comparação
    printf("\nComparing results...");
    int matrixes_are_equal = compare_double_matrixes_on_files("mR_1.dat", "mR_2.dat", NLINES, NCOLS);
    if (matrixes_are_equal) {
        printf("\nOK! Serial and parallel outputs are equal!\n");
    } else {
        printf("\nERROR! Files are NOT equal!\n");
    }

    // Métricas de desempenho
    double speedup = time_serial / time_parallel;
    int num_threads = 0;
    omp_set_num_threads(nTHREAD);
    #pragma omp parallel
    {
        #pragma omp master
        num_threads = omp_get_num_threads();
    }
    double eficiencia = speedup / num_threads;

    printf("\nSpeedup: %.2f\n", speedup);
    printf("Eficiência:  %.2f (com %d threads)\n", eficiencia, num_threads);

    free(m1);
    free(m2);
    free(mR_serial);
    free(mR_parallel);

    return 0;
}
