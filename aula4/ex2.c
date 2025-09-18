#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define N 5

pthread_barrier_t barreira;

void* worker(void* arg) {
    int id = *(int*)arg;

    printf("A");

    //faz a barreira
    pthread_barrier_wait(&barreira);

    //depois da barreira
    printf("B");

    return NULL;
}

int main() {
    pthread_t th[N];
    // declara array de ids
    int ids[N];

    // Inicializa a barreira para N participantes
    pthread_barrier_init(&barreira, NULL, N);   

    //cria a thread
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&th[i], NULL, worker, &ids[i]);
    }

    //join em todas as threads
    for (int i = 0; i < N; i++) pthread_join(th[i], NULL);

    //destroy
    pthread_barrier_destroy(&barreira);
    return 0;
}
