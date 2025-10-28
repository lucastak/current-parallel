/* =========================================================================
   EXERCÍCIO 11 (Programação) — Prevenção de Deadlocks (Filósofos)
   Enunciado:
   "Implemente o método de prevenção de deadlocks para a solução errada do
    problema do Jantar dos Filósofos. Dica: pense na ordem de alocação de
    garfos para os filósofos."
   (Aqui aplicamos ordem global: pegar sempre primeiro o garfo de menor
    índice e depois o de maior, quebrando a espera circular.)
   ========================================================================= */
// gcc -O2 -pthread diners_prevent.c -o diners_prevent
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5
pthread_mutex_t chop[N];

void* philosopher(void* idp){
  int id = *(int*)idp, L = id, R = (id+1)%N;
  int first = L < R ? L : R;
  int second = L < R ? R : L;
  while (1) {
    usleep(100000);                       // pensar
    pthread_mutex_lock(&chop[first]);
    pthread_mutex_lock(&chop[second]);
    usleep(200000);                       // comer
    pthread_mutex_unlock(&chop[second]);
    pthread_mutex_unlock(&chop[first]);
  }
  return NULL;
}

int main(void){
  for(int i=0;i<N;i++) pthread_mutex_init(&chop[i],NULL);
  pthread_t th[N]; int ids[N];
  for(int i=0;i<N;i++){ ids[i]=i; pthread_create(&th[i],NULL,philosopher,&ids[i]); }
  for(int i=0;i<N;i++) pthread_join(th[i],NULL);
  return 0;
}
