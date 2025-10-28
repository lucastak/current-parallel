/* =========================================================================
   EXERCÍCIO 13
   Enunciado:
   "Escreva um programa multithread que compute as seguintes estatísticas
    acerca de uma lista de números: a) Soma b) Média
    O programa deverá criar uma thread para cada uma das estatísticas acima,
    sendo que a média deverá reutilizar o valor calculado anteriormente pela
    thread de soma."
   ========================================================================= */
// gcc -O2 -pthread sum_mean.c -o sum_mean
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct { double* a; int n; } Data;
double gsum = 0, gmean = 0; int done = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cv  = PTHREAD_COND_INITIALIZER;

void* tsum(void* d_){
  Data*d=d_; double s=0; for(int i=0;i<d->n;i++) s+=d->a[i];
  pthread_mutex_lock(&mtx); gsum=s; done=1; pthread_cond_broadcast(&cv); pthread_mutex_unlock(&mtx);
  return NULL;
}
void* tmean(void* d_){
  Data*d=d_;
  pthread_mutex_lock(&mtx);
  while(!done) pthread_cond_wait(&cv,&mtx);
  gmean = gsum / d->n;
  pthread_mutex_unlock(&mtx);
  return NULL;
}
int main(int argc,char**argv){
  if(argc<2){ fprintf(stderr,"uso: %s n1 n2 ...\n",argv[0]); return 1; }
  int n=argc-1; double* a=malloc(sizeof(double)*n);
  if (!a) return 1;
  for(int i=0;i<n;i++) a[i]=atof(argv[i+1]);
  Data d={a,n}; pthread_t S,M;
  pthread_create(&S,NULL,tsum,&d);
  pthread_create(&M,NULL,tmean,&d);
  pthread_join(S,NULL); pthread_join(M,NULL);
  printf("soma=%.3f media=%.3f\n", gsum, gmean);
  free(a); return 0;
}
