#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 8

typedef struct {


} barreira_t;

void inicializar_barreira( barreira_t *b , int valor);
void esperar_barreira( barreira_t *b );
void destruir_barreira( barreira_t *b);

typedef struct{

	barreira_t barreira;

} shared_data_t;

typedef struct{
    int thread_id;
    shared_data_t *shared_data_p;
} thread_data_t;

void *Thread(void *);

int main(){
    thread_data_t thread_data[NTHREADS];
    shared_data_t shared_data;


    inicializar_barreira( &shared_data.barreira, NTHREADS);

	pthread_t threads[ NTHREADS ];

	for (int i=0; i<NTHREADS; i++){
        thread_data[i].thread_id = i;
        thread_data[i].shared_data_p = &shared_data;

        pthread_create(&threads[i], NULL, Thread, &thread_data[i]);
    }

    for (int i = 0; i < NTHREADS; i++){
        pthread_join(threads[i], NULL);
    }

    destruir_barreira( &shared_data.barreira );
}

void *Thread(void* thread_data){
    int thread_id=((thread_data_t*)thread_data)->thread_id;
    shared_data_t *shared_data_p=((thread_data_t*)thread_data)->shared_data_p;

    printf("Eu a Thread %d\n",thread_id);
    fflush(stdout);

    esperar_barreira( &shared_data_p->barreira );

    printf("Thread %d passou da barreira\n",thread_id);
    fflush(stdout);

	return NULL;
}


void inicializar_barreira( barreira_t *b , int valor){

}

void esperar_barreira( barreira_t *b ){


}

void destruir_barreira( barreira_t *b){


}
