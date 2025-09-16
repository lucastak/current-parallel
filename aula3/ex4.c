// exercicio4.c
// Demonstra que usar uma variável de trava "simples" NÃO garante exclusão mútua.
// Duas threads entram na "seção crítica" ao mesmo tempo e sobrescrevem um slot do vetor.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

volatile int trava = 0;         // trava "caseira" (INCORRETA)
int indice_compartilhado = 0;   // posição a escrever no vetor
int vetor[2] = { -1, -1 };      // pequeno para visualizarmos a sobrescrita

void* trabalhador(void* arg) {
  int id = *(int*)arg;

  // --- "Entrada" na seção crítica com trava defeituosa ---
  // Duas janelas de corrida:
  // 1) Ambas leem trava==0
  // 2) Ambas escrevem trava=1
  while (trava == 1) { /* busy wait */ }
  // Força uma condição de corrida: deixa tempo para outra thread passar desse mesmo ponto
  usleep(1000);
  trava = 1;

  // --- Seção crítica (deveria ser exclusiva, mas não é garantido) ---
  int pos = indice_compartilhado;  // ambas podem ler o mesmo valor!
  printf("[t%d] Entrou na secao critica. indice_compartilhado=%d\n", id, pos);
  usleep(2000); // tempo para a outra thread também entrar e usar a MESMA posicao
  vetor[pos] = id;                  // sobrescreve potencialmente o mesmo slot
  indice_compartilhado++;
  printf("[t%d] Escreveu vetor[%d]=%d e incrementou indice_compartilhado -> %d\n",
         id, pos, id, indice_compartilhado);

  // --- Saída ---
  trava = 0;
  return NULL;
}

int main(void) {
  pthread_t t1, t2;
  int id1 = 1, id2 = 2;

  printf("[MAIN] Iniciando demonstracao de falha da 'trava' simples.\n");

  pthread_create(&t1, NULL, trabalhador, &id1);
  pthread_create(&t2, NULL, trabalhador, &id2);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("[MAIN] Resultado vetor: [%d, %d]\n", vetor[0], vetor[1]);
  printf("[MAIN] indice_compartilhado final: %d (pode ser 2, mas com sobrescrita!)\n");

  printf("\nConclusao: houve janela de corrida. Ambas as threads entraram na 'seção crítica' "
         "quase juntas, leram o mesmo indice_compartilhado e sobrescreveram o mesmo slot.\n"
         "Solucoes corretas: mutex (pthread_mutex), atomicos (C11 _Atomic), semaforos, ou algoritmos "
         "teoricamente corretos (Dekker, Peterson) – nao esta variavel simples.\n");
  return 0;
}
