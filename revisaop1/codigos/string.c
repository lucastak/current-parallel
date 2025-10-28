/* =========================================================================
   Frequencia de uma letra em uma string com N threads
   Enunciado:
   "Conte a frequência de uma letra em uma string. Particione em N
    segmentos e some os resultados."
   ========================================================================= */
// gcc -O2 -pthread count_char_min.c -o count_char_min
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char* s;
  long ini, fim;
  char alvo;
  int id;
  long parcial;
} Arg;

void* worker(void* a_) {
  Arg* a = (Arg*)a_;
  long c = 0;
  for (long i = a->ini; i < a->fim; i++) {
    if (a->s[i] == a->alvo) c++;
  }
  a->parcial = c;
  printf("[Thread %d] Segmento [%ld:%ld] => %ld '%c'\n",
         a->id, a->ini, a->fim, c, a->alvo);
  return NULL;
}

int main(void) {
  const char* s = "erro e terror";
  char alvo = 'r';
  int T = 4;

  long L = strlen(s);
  long base = L / T, extra = L % T, pos = 0;
  long total = 0;

  pthread_t th[T];
  Arg args[T];

  for (int t = 0; t < T; t++) {
    long len = base + (t < extra);
    args[t] = (Arg){ s, pos, pos + len, alvo, t, 0 };
    pos += len;
    pthread_create(&th[t], NULL, worker, &args[t]);
  }

  for (int t = 0; t < T; t++) {
    pthread_join(th[t], NULL);
    total += args[t].parcial;
  }

  printf("\n[Resultado final] Ocorrências de '%c' = %ld\n", alvo, total);
  return 0;
}
