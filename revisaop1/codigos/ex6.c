/* =========================================================================
   “AAAAABBBBB” COM 5 THREADS
   Enunciado:
   "Considere um programa com 5 threads que executarão:
      void thread(thread_id i) { ... write('A') ... write('B') ... }
    Implemente utilizando semáforos ou outro mecanismo de sincronização de
    forma que sempre imprima 'AAAAABBBBB' na tela."
   ========================================================================= */
// gcc -O2 -pthread aaaaa_bbbbb.c -o aaaaa_bbbbb
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

sem_t semB;
int countA = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void* th(void* _) {
  // fase A
  putchar('A');
  fflush(stdout);
  pthread_mutex_lock(&mtx);
  countA++;
  if (countA == 5) { for (int i=0;i<5;i++) sem_post(&semB); }
  pthread_mutex_unlock(&mtx);

  // fase B (espera liberação)
  sem_wait(&semB);
  putchar('B');
  fflush(stdout);
  return NULL;
}

int main(){
  sem_init(&semB,0,0);
  pthread_t t[5];
  for(int i=0;i<5;i++) pthread_create(&t[i],NULL,th,NULL);
  for(int i=0;i<5;i++) pthread_join(t[i],NULL);
  putchar('\n');
  sem_destroy(&semB);
  return 0;
}
