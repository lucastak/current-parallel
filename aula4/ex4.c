#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 8

typedef struct {
    int N;          // quantidade de threads que sincronizam
    int count;      // contador de chegadas
    sem_t mutex;    // exclusão mútua para 'count'
    sem_t portao1;   // portao 1
    sem_t portao2;  // portao 2 (reset)
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

    inicializar_barreira( &shared_data.barreira, NTHREADS );

    pthread_t threads[NTHREADS];
    for (int i=0; i<NTHREADS; i++){
        thread_data[i].thread_id = i;
        thread_data[i].shared_data_p = &shared_data;
        pthread_create(&threads[i], NULL, Thread, &thread_data[i]);
    }

    for (int i=0; i<NTHREADS; i++){
        pthread_join(threads[i], NULL);
    }

    destruir_barreira( &shared_data.barreira );
    return 0;
}

void *Thread(void* arg){
    int thread_id = ((thread_data_t*)arg)->thread_id;
    shared_data_t *shared_data_p = ((thread_data_t*)arg)->shared_data_p;

    printf("thread %d chegou antes da barreira\n", thread_id);
    fflush(stdout);

    esperar_barreira( &shared_data_p->barreira );

    printf("thread %d passou da barreira\n", thread_id);
    fflush(stdout);;

    return NULL;
}

void inicializar_barreira( barreira_t *b , int valor){
    //qtd thread para sincronizar na barreira
    b->N = valor;
    b->count = 0;

    //inicia semafaro
    sem_init(&b->mutex, 0, 1);

    //portao 1 fechado
    // nao passa ate todas chegarem
    sem_init(&b->portao1, 0, 0);

    // abre portao 2
    sem_init(&b->portao2, 0, 1);
}

void esperar_barreira( barreira_t *b ){
    // regiao critia, atualiza contador
    sem_wait(&b->mutex);
    b->count++;

    // verifica se todas chegaram
    if (b->count == b->N) {
        // Fecha o portao 2 para que nenhuma thread do próximo ciclo avance antes da hora
        // fecha portao 2. nenhuma avanca antes da hora
        sem_wait(&b->portao2);

        // abre portao pras threads esperando
        for (int i = 0; i < b->N; i++) {
            sem_post(&b->portao1);
        }
    }

    // libera regiao critia
    sem_post(&b->mutex);
    // passa portao 1
    sem_wait(&b->portao1);
    // permite que todas atravessem
    sem_post(&b->portao1);

    //RESETAR BARREIRA
    // Região crítica para decrementar o contador
    sem_wait(&b->mutex);

    // a thread ta saindo da barreira, decrementa
    b->count--;

    // foi a ultima a sair?
    if (b->count == 0) {
        //fecha o portao pro proximo uso
        sem_wait(&b->portao1);

        //abre portao 2 pro proximo uso
        for (int i = 0; i < b->N; i++) {
            sem_post(&b->portao2);
        }
    }

    // libera regiao critica
    sem_post(&b->mutex);

    // passa pelo portao 2
    sem_wait(&b->portao2);
    // libera o portao 2 para que todas atrevessem
    sem_post(&b->portao2);
}

void destruir_barreira( barreira_t *b){
    sem_destroy(&b->mutex);
    sem_destroy(&b->portao1);
    sem_destroy(&b->portao2);
}
