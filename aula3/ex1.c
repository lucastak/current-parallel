// exercicio1.c
// Cria 23 threads. Todas compartilham a variável 'letra'.
// Cada thread só imprime quando 'letra' == sua vez; depois incrementa e acorda a próxima.
// Implementa um "monitor simples" com mutex + variável de condição.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define NUM_THREADS 23 // troque para 23 se o professor exigir exatamente 23

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

char letra_compartilhada = 'A';   // variável compartilhada 'letra'
int  total_para_imprimir = NUM_THREADS;

typedef struct {
  int indice; // 0..N-1 (A..Z)
} DadosThread;

void* rotina_impressao(void* arg) {
  DadosThread* dados = (DadosThread*)arg;
  char minha_letra = 'A' + dados->indice;

  pthread_mutex_lock(&mutex);
  while (letra_compartilhada != minha_letra) {
    // LOG: não é minha vez ainda
    printf("[t%02d] Aguardando: minha_letra=%c, atual=%c\n",
           dados->indice, minha_letra, letra_compartilhada);
    pthread_cond_wait(&cond, &mutex);
  }

  // É a minha vez
  printf("[t%02d] >>> IMPRIMINDO letra: %c\n", dados->indice, minha_letra);
  // Simula algum trabalho
  usleep(20 * 3000);

  // Incrementa para a próxima thread/letra
  letra_compartilhada++;
  total_para_imprimir--;
  printf("[t%02d] Incrementou 'letra' para: %c | faltam: %d\n",
         dados->indice, letra_compartilhada, total_para_imprimir);

  // Acorda geral (a próxima interessada vai prosseguir)
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mutex);

  return NULL;
}

int main(void) {
  pthread_t threads[NUM_THREADS];
  DadosThread dados[NUM_THREADS];

  printf("[MAIN] Iniciando %d threads. Começando em letra = %c\n",
         NUM_THREADS, letra_compartilhada);

  for (int i = 0; i < NUM_THREADS; i++) {
    dados[i].indice = i;
    if (pthread_create(&threads[i], NULL, rotina_impressao, &dados[i]) != 0) {
      perror("pthread_create");
      exit(1);
    }
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("[MAIN] Finalizado. Última letra foi: %c (deve ser 'A' + %d)\n",
         letra_compartilhada, NUM_THREADS);
  return 0;
}
