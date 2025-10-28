/* =========================================================================
   PROGRAMA 1 — "error" com 1 thread por tipo de letra
   Enunciado:
   "Escreva um programa concorrente que imprima a palavra 'error'.
    Deverá haver uma thread para cada letra. Mas só poderá existir uma
    thread para imprimir cada tipo de letra, ou seja, não pode haver 2
    threads imprimindo a letra 'r'."

   Observação:
   - Temos 3 letras distintas: 'e', 'r', 'o'.
   - Criamos 3 threads: Te, Tr e To.
   - Usamos mutex + variável de condição para garantir a ordem 'e','r','r','o','r'.
   ========================================================================= */
// gcc -O2 -pthread print_error.c -o print_error
#include <pthread.h>
#include <stdio.h>
#include <string.h>

static const char* alvo = "error";
static int idx = 0; // próximo caractere a imprimir
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cv  = PTHREAD_COND_INITIALIZER;

typedef struct { char letra; } Arg;

void* worker(void* a_) {
  Arg* a = (Arg*)a_;
  int n = (int)strlen(alvo);
  while (1) {
    pthread_mutex_lock(&mtx);
    while (idx < n && alvo[idx] != a->letra) {
      pthread_cond_wait(&cv, &mtx);
    }
    if (idx >= n) { // acabou
      pthread_mutex_unlock(&mtx);
      break;
    }
    // É a minha vez
    putchar(a->letra);
    fflush(stdout);
    idx++;
    pthread_cond_broadcast(&cv);
    pthread_mutex_unlock(&mtx);

    if (idx >= n) break;
  }
  return NULL;
}

int main(void) {
  pthread_t te, tr, to;
  Arg Ae = { 'e' }, Ar = { 'r' }, Ao = { 'o' };

  pthread_create(&te, NULL, worker, &Ae);
  pthread_create(&tr, NULL, worker, &Ar);
  pthread_create(&to, NULL, worker, &Ao);

  pthread_join(te, NULL);
  pthread_join(tr, NULL);
  pthread_join(to, NULL);

  putchar('\n');
  return 0;
}
