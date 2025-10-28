/* =========================================================================
   CORRIDA DE THREADS
   Enunciado (resumo):
   "Simule uma corrida de threads. O usuário entra com a quantidade de
    threads. Todas devem esperar pela criação de todas (largada da main).
    Ao chegar, cada thread imprime id e colocação. Após as 3 primeiras
    chegarem, o programa termina sem que as demais imprimam."
   ========================================================================= */
// gcc -O2 -pthread corrida.c -o corrida
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

typedef struct { int id; } Arg;
int coloc = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
volatile int start_flag = 0;

void* runner(void* a_) {
  Arg* a=(Arg*)a_;
  while(!start_flag) sched_yield();         // largada simples

  usleep( (rand()%1000 + 200) * 1000 );     // “corrida”

  pthread_mutex_lock(&mtx);
  if (coloc < 3) {
    coloc++;
    printf("Thread %d chegou! Colocacao: %d\n", a->id, coloc);
  }
  pthread_mutex_unlock(&mtx);
  return NULL;
}

int main(int argc,char**argv){
  if(argc<2){ fprintf(stderr,"uso: %s Nthreads\n", argv[0]); return 1; }
  int N=atoi(argv[1]); srand((unsigned)time(NULL));
  pthread_t* th = malloc(sizeof(pthread_t)*N);
  Arg* args = malloc(sizeof(Arg)*N);
  for (int i=0;i<N;i++){ args[i].id=i; pthread_create(&th[i],NULL,runner,&args[i]); }

  start_flag = 1; // largada após criação

  // encerra quando 3 chegarem
  while (1) {
    pthread_mutex_lock(&mtx); int done = (coloc>=3); pthread_mutex_unlock(&mtx);
    if (done) break;
    usleep(1000);
  }
  printf("Corrida encerrada apos 3 chegadas.\n");
  exit(0); // sai sem esperar as demais
}
