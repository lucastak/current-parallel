/* =========================================================================
   EXERCÍCIO 10 (Programação) — Detector no Jantar dos Filósofos
   Enunciado:
   "Implemente o algoritmo do banqueiro para a solução errada para o
    problema do Jantar dos Filósofos. Para isso, crie uma thread chamada
    'Detector', por exemplo."
   (Aqui implementamos uma solução ingênua propensa a deadlock e uma
    thread Detector que sinaliza quando todos seguram 1 garfo e esperam.)
   ========================================================================= */
// gcc -O2 -pthread diners_detect.c -o diners_detect
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5
pthread_mutex_t chop[N];
volatile int holding[N] = {0};     // 0 nenhum, 1 um garfo, 2 ambos
volatile int waiting[N] = {0};     // esperando pelo segundo garfo

void* philosopher(void* idp){
  int id = *(int*)idp, L = id, R = (id+1)%N;
  while (1) {
    usleep(100000); // pensar
    pthread_mutex_lock(&chop[L]); holding[id]=1;
    waiting[id]=1;
    pthread_mutex_lock(&chop[R]); holding[id]=2; waiting[id]=0;
    usleep(200000); // comer
    pthread_mutex_unlock(&chop[R]);
    pthread_mutex_unlock(&chop[L]);
    holding[id]=0;
  }
  return NULL;
}

void* detector(void* _){
  while (1) {
    usleep(500000);
    int all_waiting = 1;
    for (int i=0;i<N;i++) {
      if (!(holding[i]==1 && waiting[i]==1)) { all_waiting=0; break; }
    }
    if (all_waiting) {
      printf("DEADLOCK detectado! (todos com 1 garfo, esperando o outro)\n");
      exit(0);
    }
  }
  return NULL;
}

int main(void){
  for(int i=0;i<N;i++) pthread_mutex_init(&chop[i],NULL);
  pthread_t th[N], det; int ids[N];
  for(int i=0;i<N;i++){ ids[i]=i; pthread_create(&th[i],NULL,philosopher,&ids[i]); }
  pthread_create(&det,NULL,detector,NULL);
  pthread_join(det,NULL);
  return 0;
}
