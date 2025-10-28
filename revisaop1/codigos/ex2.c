/* =========================================================================
   EXERCÍCIO 11
   Enunciado:
   "Escreva um programa multithread que compute quantas vezes um número
    ocorre em um vetor de N posições. O seu programa deverá funcionar
    para um número qualquer de threads."
   ========================================================================= */
// gcc -O2 -pthread count_occ.c -o count_occ
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { int *v; long ini, fim; int alvo; long parcial; } Arg;

void* worker(void* a_) {
  Arg* a = (Arg*)a_;
  long c = 0;
  for (long i = a->ini; i < a->fim; i++) if (a->v[i] == a->alvo) c++;
  a->parcial = c;
  return NULL;
}

int main(int argc, char** argv) {
  if (argc < 4) { fprintf(stderr, "uso: %s N T alvo\n", argv[0]); return 1; }
  long N = atol(argv[1]), T = atol(argv[2]); int alvo = atoi(argv[3]);
  int* v = malloc(sizeof(int)*N);
  if (!v) { perror("malloc"); return 1; }
  for (long i = 0; i < N; i++) v[i] = rand()%10; // exemplo simples

  pthread_t* th = malloc(sizeof(pthread_t)*T);
  Arg* args = malloc(sizeof(Arg)*T);
  long bloco = (N + T - 1)/T;

  for (long t = 0; t < T; t++) {
    args[t] = (Arg){ v, t*bloco, (t+1)*bloco < N ? (t+1)*bloco : N, alvo, 0 };
    pthread_create(&th[t], NULL, worker, &args[t]);
  }

  long total = 0;
  for (long t = 0; t < T; t++) { pthread_join(th[t], NULL); total += args[t].parcial; }
  printf("ocorrencias de %d = %ld\n", alvo, total);
  free(args); free(th); free(v);
  return 0;
}
