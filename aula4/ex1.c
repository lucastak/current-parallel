#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_barrier_t barreira1;
pthread_barrier_t barreira2;
pthread_barrier_t barreira3;

void* threadC(void* arg) {
    printf("C");
    // libera thread A
    pthread_barrier_wait(&barreira1);
    return NULL;
}

void* threadA(void* arg) {
    // espera C
    pthread_barrier_wait(&barreira1);
    printf("A");
    // libera thread F
    pthread_barrier_wait(&barreira2);
    return NULL;
}

void* threadF(void* arg) {
    // espera A
    pthread_barrier_wait(&barreira2);
    printf("F");
    // libera thread E
    pthread_barrier_wait(&barreira3);
    return NULL;
}

void* threadE(void* arg) {
    // espera F
    pthread_barrier_wait(&barreira3); 
    printf("E\n");
    return NULL;
}

int main() {
    pthread_t tC, tA, tF, tE;

    // inicia as barreiras
    pthread_barrier_init(&barreira1, NULL, 2); 
    pthread_barrier_init(&barreira2, NULL, 2);
    pthread_barrier_init(&barreira3, NULL, 2);

    pthread_create(&tC, NULL, threadC, NULL);
    pthread_create(&tA, NULL, threadA, NULL);
    pthread_create(&tF, NULL, threadF, NULL);
    pthread_create(&tE, NULL, threadE, NULL);

    //espera a threads
    pthread_join(tC, NULL);
    pthread_join(tA, NULL);
    pthread_join(tF, NULL);
    pthread_join(tE, NULL);

    // destroi recurso
    pthread_barrier_destroy(&barreira1);
    pthread_barrier_destroy(&barreira2);
    pthread_barrier_destroy(&barreira3);

    return 0;
}
