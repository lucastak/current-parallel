/* =========================================================================
   QUESTÃO 1
   Enunciado:
   "Printar todas as letras do alfabeto usando 23 threads.
    Há uma variável compartilhada 'letra'. A cada impressão, a 'próxima
    letra' deve ser impressa pela 'próxima thread' (ordem de threads
    em round-robin)."
   ========================================================================= */
// gcc -O2 -pthread alfabeto_23threads.c -o alfabeto_23threads
#include <pthread.h>
#include <stdio.h>

#define T 23

char letra = 'a';
int turno = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cv  = PTHREAD_COND_INITIALIZER;

void* worker(void* arg){
  int id = (int)(long)arg;
  for(;;){
    pthread_mutex_lock(&mtx);
    while (letra <= 'z' && turno != id) pthread_cond_wait(&cv, &mtx);
    if (letra > 'z'){ pthread_mutex_unlock(&mtx); break; }

    putchar(letra); fflush(stdout);
    letra++;
    turno = (turno + 1) % T;

    pthread_cond_broadcast(&cv);
    pthread_mutex_unlock(&mtx);
  }
  return NULL;
}

int main(void){
  pthread_t th[T];
  for (int i=0;i<T;i++) pthread_create(&th[i], NULL, worker, (void*)(long)i);
  for (int i=0;i<T;i++) pthread_join(th[i], NULL);
  putchar('\n');
  return 0;
}
