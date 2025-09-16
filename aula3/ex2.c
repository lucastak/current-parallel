// exercicio2.c
// Soma de todos os elementos de um vetor de inteiros usando UMA variável compartilhada.
// Cada thread atualiza 'soma_total' sob mutex, mostrando logs.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
  int* vetor;
  int inicio; // índice inicial (inclusivo)
  int fim;    // índice final (exclusivo)
  int id;
} DadosThread;

long long soma_total = 0;                 // variável compartilhada
pthread_mutex_t mutex_soma = PTHREAD_MUTEX_INITIALIZER;

void* soma_faixa(void* arg) {
  DadosThread* d = (DadosThread*)arg;

  for (int i = d->inicio; i < d->fim; i++) {
    // Simula custo
    // usleep(1000);
    pthread_mutex_lock(&mutex_soma);
    long long antes = soma_total;
    soma_total += d->vetor[i];
    long long depois = soma_total;
    printf("[t%02d] i=%d, valor=%d | soma_total: %lld -> %lld\n",
           d->id, i, d->vetor[i], antes, depois);
    pthread_mutex_unlock(&mutex_soma);
  }
  return NULL;
}

int main(void) {
  const int N = 100;
  const int T = 4; // número de threads

  int* v = malloc(sizeof(int) * N);
  if (!v) { perror("malloc"); return 1; }

  // Preenche vetor com 1..N
  for (int i = 0; i < N; i++) v[i] = i + 1;

  pthread_t th[T];
  DadosThread dados[T];

  int base = N / T;
  int resto = N % T;
  int ini = 0;

  for (int t = 0; t < T; t++) {
    int tam = base + (t < resto ? 1 : 0);
    dados[t].vetor = v;
    dados[t].inicio = ini;
    dados[t].fim = ini + tam;
    dados[t].id = t;
    ini += tam;

    if (pthread_create(&th[t], NULL, soma_faixa, &dados[t]) != 0) {
      perror("pthread_create"); return 1;
    }
  }

  for (int t = 0; t < T; t++) pthread_join(th[t], NULL);

  // Valor esperado: soma de 1..N = N*(N+1)/2
  long long esperado = (long long)N * (N + 1) / 2;
  printf("[MAIN] soma_total = %lld | esperado = %lld\n", soma_total, esperado);

  free(v);
  return 0;
}
