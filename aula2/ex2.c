/*
    Implemente um algoritmo paralelo para realizar a soma de dois vetores indice-a-indice de
    um vetor de inteiros. Salve o resultado da soma no primeiro vetor
*/
/*
  Compilar:  gcc -O2 -pthread ex2.c -o ex2
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 100
#define NUM_THREADS 4

typedef struct {
    int *a;
    int *b;
    int inicio;
    int fim;
} shared_memo_t;

void* worker(void *arg) {
    shared_memo_t *args = (shared_memo_t*) arg;
    int *a = args->a;
    int *b = args->b;
    for (int i = args->inicio; i < args->fim; i++) {
        a[i] += b[i];
    }
    pthread_exit(NULL);
}

int main(void) {
    // Aloca e inicializa vetores
    int *a = (int*) malloc(SIZE * sizeof(int));
    int *b = (int*) malloc(SIZE * sizeof(int));
    if (!a || !b) {
        fprintf(stderr, "Falha de alocacao\n");
        return 1;
    }
    for (int i = 0; i < SIZE; i++) {
        a[i] = i;   // 0,1,2,3,...
        b[i] = i;   // 0,1,2,3,...
    }

    // Cria threads e distribui o trabalho em blocos
    pthread_t threads[NUM_THREADS];
    shared_memo_t args[NUM_THREADS];

    int chunk = (SIZE + NUM_THREADS - 1) / NUM_THREADS; // ceil
    for (int t = 0; t < NUM_THREADS; t++) {
        int inicio = t * chunk;
        int fim   = inicio + chunk;
        if (fim > SIZE) fim = SIZE;

        args[t].a = a;
        args[t].b = b;
        args[t].inicio = inicio;
        args[t].fim = fim;

        // Evita criar thread vazia (quando SIZE < NUM_THREADS)
        if (inicio < fim) {
            int rc = pthread_create(&threads[t], NULL, worker, &args[t]);
            if (rc != 0) {
                fprintf(stderr, "Erro ao criar thread %d (rc=%d)\n", t, rc);
                return 1;
            }
        } else {
            threads[t] = (pthread_t)0; // marcador de “não criada”
        }
    }

    // Espera (join) todas as threads terminarem
    for (int t = 0; t < NUM_THREADS; t++) {
        if (threads[t]) {
            pthread_join(threads[t], NULL);
        }
    }

    // (Opcional) imprime alguns resultados para verificar
    for (int i = 0; i < (SIZE < 20 ? SIZE : 20); i++) {
        printf("%d ", a[i]);
    }
    printf("\n");

    free(a);
    free(b);
    return 0;
}
