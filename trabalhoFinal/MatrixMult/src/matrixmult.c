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

#define NLINES 3
#define NCOLS 3

// Descomente esta linha abaixo para imprimir valores das matrizes 
//#define __DEBUG__

enum implementations_enum {
	TYPE_SERIAL = 1, 
	TYPE_PARALLEL
} ;

double *MatrixMult_serial(const double *m1, const double *m2){

	double *mR = (double*)malloc(
		sizeof(double) * NLINES * NCOLS);

	for ( long int i = 0; i < NLINES; i++ ){

		for ( long int j = 0; j < NCOLS; j++ ){

			// Utilize esta macro abaixo para acessar valores de matrizes em C
			M( i , j , NCOLS, mR) = 0;

			for (long int k = 0; k < NCOLS; k++ ){

				M(i, j, NCOLS, mR) += 
					M( i , k , NCOLS, m1) * M( k , j , NCOLS, m2);

			}

		}
	}
	
	return mR;
}


double *MatrixMult_parallel(const double *m1, const double *m2){

	double *mR = (double*)malloc(
		sizeof(double) * NLINES * NCOLS);

	for ( long int i = 0; i < NLINES; i++ ){

		for ( long int j = 0; j < NCOLS; j++ ){

			// Utilize esta macro abaixo para acessar valores de matrizes em C
			M( i , j , NCOLS, mR) = 0;

			for (long int k = 0; k < NCOLS; k++ ){

				M(i, j, NCOLS, mR) += 
					M( i , k , NCOLS, m1) * M( k , j , NCOLS, m2);

			}

		}
	}
	
	return mR;
}


int main(int argc, char ** argv){

	srand( time(NULL) );

	/**
	 * 
	 * Matrizes em C precisam ser alocadas como vetores 
	 * acessadas pela regra M(i,j) = M[ i* colunas + j ]
	 * 
	 * */
	double *m1, *m2, *mR;
	
	int implementation_to_use;

	if (access("m1.dat", F_OK) != 0) {
	
		printf("\nGenerating new Matrix 1 values...");
		m1 = (double*)generate_random_double_matrix( NLINES, NCOLS );
		
		save_double_matrix( m1, NLINES, NCOLS, "m1.dat");
		
		implementation_to_use = TYPE_SERIAL;
					

	} else {
	
		printf("\nLoading Matrix 1 from file ...");
		m1 = load_double_matrix("m1.dat", NLINES, NCOLS);
		
	}

#ifdef __DEBUG__
	print_double_matrix( m1 , NLINES, NCOLS);
#endif

	if (access("m2.dat", F_OK) != 0) {
	
		printf("\nGenerating new Matrix 2 values...");
		m2 = (double*)generate_random_double_matrix( NLINES, NCOLS );
		
		save_double_matrix( m2, NLINES, NCOLS, "m2.dat");
		
		implementation_to_use = TYPE_SERIAL;		
					

	} else {

		printf("\nLoading Matrix 2 from file ...");
		m2 = load_double_matrix("m2.dat", NLINES, NCOLS);
		
		implementation_to_use = TYPE_PARALLEL;

	}

#ifdef __DEBUG__
	print_double_matrix( m2 , NLINES, NCOLS);
#endif
	
	/* 
		Execute o programa da primeira vez na forma serial. 
		Ele ira gerar automaticamente as matrizes m1 e m2, as quais serão salvas e usadas automaticamente 
		nas execucoes futuras				
	*/
	
	if (implementation_to_use == TYPE_SERIAL){
	
		fprintf(stdout, "\nRunning serial implementation ...");
		mR = MatrixMult_serial( m1, m2 );		
			
	} else if (implementation_to_use == TYPE_PARALLEL){
	
		fprintf(stdout, "\nRunning parallel implementation ...");
		mR = MatrixMult_parallel( m1, m2 );
	}

#ifdef __DEBUG__
	printf("\nResulting matrix multiplication:");
	print_double_matrix( mR , 3, 3);
#endif	

	/* 
		Quando voce executar o programa da primeira vez com a implementacao serial, 
		ele ira criar o arquivo mR_1.dat.
		
		Nas execucoes seguintes, utilize a versao paralela. 
		Ele entao ira criar o arquivo mR_2.dat e comparar com o anterior a fim de verificar se 
		a implementacao serial e paralela estao dando as mesmas respostas
	*/

	if (access("mR_1.dat", F_OK) != 0) {

		/*
		Primeira execucao (serial), salva a resposta
		*/
		save_double_matrix( mR, NLINES, NCOLS, "mR_1.dat");
		
		printf("\nSerial result saved");


	} else {
		
		/* 
			Segunda e sucessivas execucoes (paralelas): salva a resposta e compara com a anterior
		*/
		save_double_matrix( mR, NLINES, NCOLS, "mR_2.dat");
		
		printf("\nParallel result saved - comparing serial and parallel outputs ...");

		int matrixes_are_equal = compare_double_matrixes_on_files("mR_1.dat", 
			"mR_2.dat",
			NLINES,
			NCOLS);
		
		if ( matrixes_are_equal ){
			printf("\nOK! Serial and parallel outputs are equal!");
		} else {
			printf("\nERROR! Files are NOT equal! Something is wrong (probably on parallel version)!");
		}	
	}
	
	printf("\n");

	free( m1 );
	free( m2 );
	free( mR );

	return 0;
}
