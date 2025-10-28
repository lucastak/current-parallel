//Considere uma variável compartilhada valor e 
//duas threads chamadas Incrementador e Decrementador as quais, respectivamente,
// incrementam e decrementam essa variável N vezes.
//Escreva um programa concorrente que retorne o valor final
//dessa variável ao final dessa N iterações.
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static long valor = 0;
static long N;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void* inc(void* _) {
  for (long i = 0; i < N; i++) {
    pthread_mutex_lock(&mtx);
    valor++;
    pthread_mutex_unlock(&mtx);
  }
  return NULL;
}
void* dec(void* _) {
  for (long i = 0; i < N; i++) {
    pthread_mutex_lock(&mtx);
    valor--;
    pthread_mutex_unlock(&mtx);
  }
  return NULL;
}

int main(int argc, char** argv) {
  if (argc < 2) { fprintf(stderr, "uso: %s N\n", argv[0]); return 1; }
  N = atol(argv[1]);
  pthread_t t1, t2;
  pthread_create(&t1, NULL, inc, NULL);
  pthread_create(&t2, NULL, dec, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  printf("valor final = %ld\n", valor);
  return 0;
}
