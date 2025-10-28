/* =========================================================================
   EXERCÍCIO 12
   Enunciado:
   "Escreva um programa multithread que compute as seguintes estatísticas
    acerca de uma lista de números: a) Maior número b) Menor número c) Média
    O programa deverá receber uma lista de números pela linha de comando e
    então criar 3 threads, uma para cada estatística acima."
   ========================================================================= */
// gcc -O2 -pthread stats3.c -o stats3
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct { double* a; int n; } Data;
double gmax, gmin, gavg;

void* tmax(void* d_){ Data*d=d_; gmax=d->a[0]; for(int i=1;i<d->n;i++) if(d->a[i]>gmax) gmax=d->a[i]; return NULL; }
void* tmin(void* d_){ Data*d=d_; gmin=d->a[0]; for(int i=1;i<d->n;i++) if(d->a[i]<gmin) gmin=d->a[i]; return NULL; }
void* tavg(void* d_){ Data*d=d_; double s=0; for(int i=0;i<d->n;i++) s+=d->a[i]; gavg=s/d->n; return NULL; }

int main(int argc, char** argv){
  if (argc < 2){ fprintf(stderr,"uso: %s n1 n2 ...\n", argv[0]); return 1; }
  int n = argc-1; double* a = malloc(sizeof(double)*n);
  if (!a) return 1;
  for(int i=0;i<n;i++) a[i]=atof(argv[i+1]);
  Data d={a,n}; pthread_t T1,T2,T3;
  pthread_create(&T1,NULL,tmax,&d);
  pthread_create(&T2,NULL,tmin,&d);
  pthread_create(&T3,NULL,tavg,&d);
  pthread_join(T1,NULL); pthread_join(T2,NULL); pthread_join(T3,NULL);
  printf("max=%.3f min=%.3f media=%.3f\n", gmax,gmin,gavg);
  free(a); return 0;
}
