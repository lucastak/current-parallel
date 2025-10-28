/* =========================================================================
   EXERCÍCIO 9 (Programação) — Algoritmo do Banqueiro (estado seguro)
   Enunciado:
   "Implemente o algoritmo do banqueiro para alocação segura de recursos.
    Entrada: matriz n x 2 (col0=alocado, col1=máximo por processo) e
    quantidade de recursos disponíveis N. Depois, pergunte processo p e
    quantidade de recursos solicitada. Imprima se o estado resultante é
    seguro ou não."
   ========================================================================= */
// gcc -O2 banker.c -o banker
#include <stdio.h>
#include <stdlib.h>

int safe(int n, int* alloc, int* max, int avail) {
  int work = avail;
  int* finish = malloc(sizeof(int)*n);
  if (!finish) return 0;
  for(int i=0;i<n;i++) finish[i] = 0;

  while (1) {
    int progressed = 0;
    for (int i=0;i<n;i++) {
      if (finish[i]) continue;
      int need = max[i] - alloc[i];
      if (need <= work) { work += alloc[i]; finish[i]=1; progressed=1; }
    }
    if (!progressed) break;
  }
  int ok = 1; for (int i=0;i<n;i++) if (!finish[i]) { ok=0; break; }
  free(finish);
  return ok;
}

int main(void){
  int n; if (scanf("%d",&n)!=1) return 1;
  int* alloc = malloc(sizeof(int)*n);
  int* max   = malloc(sizeof(int)*n);
  if (!alloc || !max) return 1;

  for (int i=0;i<n;i++) scanf("%d %d",&alloc[i], &max[i]);
  int avail; scanf("%d",&avail);
  int p, req; scanf("%d %d",&p,&req);

  if (p<0 || p>=n) { puts("Processo invalido."); return 0; }
  if (req > max[p]-alloc[p]) { puts("Pedido invalido (excede max)."); return 0; }
  if (req > avail) { puts("Nao ha disponivel suficiente (nao seguro)."); return 0; }

  alloc[p] += req; avail -= req;
  int ok = safe(n, alloc, max, avail);
  puts(ok ? "Estado SEGURO apos concessao." : "Estado NAO seguro apos concessao.");
  free(alloc); free(max);
  return 0;
}
