/* =========================================================================
   EXERCÍCIO 14
   Enunciado:
   "Implemente um programa que conte a frequência de ocorrências de uma
    palavra em um arquivo de texto. O texto deverá ser particionado em N
    segmentos a serem processados por N threads, e o resultado final deverá
    ser a soma de todas as ocorrências de cada segmento."
   ========================================================================= */
// gcc -O2 -pthread word_count.c -o word_count
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct { const char* buf; long ini, fim; const char* word; long count; } Arg;

int isdelim(int c){ return !isalnum(c); }
int word_eq(const char* s, const char* w, int wl){
  for (int i=0;i<wl;i++) if (tolower((unsigned char)s[i]) != tolower((unsigned char)w[i])) return 0;
  return 1;
}
void* worker(void* a_){
  Arg* a=(Arg*)a_; long c=0; int wl=strlen(a->word);
  long i=a->ini;
  while(i < a->fim){
    while(i < a->fim && isdelim(a->buf[i])) i++;
    long start=i;
    while(i < a->fim && !isdelim(a->buf[i])) i++;
    long len=i-start;
    if (len==wl && word_eq(a->buf+start, a->word, wl)) c++;
  }
  a->count=c; return NULL;
}

int main(int argc,char**argv){
  if(argc<4){ fprintf(stderr,"uso: %s arquivo palavra Nthreads\n", argv[0]); return 1; }
  const char* path=argv[1]; const char* word=argv[2]; int T=atoi(argv[3]);
  FILE* f=fopen(path,"rb"); if(!f){perror("fopen"); return 1;}
  fseek(f,0,SEEK_END); long L=ftell(f); fseek(f,0,SEEK_SET);
  char* buf=malloc(L+1); if(!buf){perror("malloc"); return 1;}
  fread(buf,1,L,f); buf[L]='\0'; fclose(f);

  pthread_t* th=malloc(sizeof(pthread_t)*T); Arg* args=malloc(sizeof(Arg)*T);
  long bloco=L/T, pos=0;
  for(int t=0;t<T;t++){
    long ini=pos, fim=(t==T-1? L : pos+bloco);
    if (ini>0) while(ini<fim && !isdelim(buf[ini-1])) ini++;  // alinhar início
    if (fim<L) while(fim>ini && !isdelim(buf[fim])) fim++;    // alinhar fim
    args[t]=(Arg){buf,ini,fim,word,0};
    pos += bloco;
    pthread_create(&th[t],NULL,worker,&args[t]);
  }
  long total=0; for(int t=0;t<T;t++){ pthread_join(th[t],NULL); total+=args[t].count; }
  printf("'%s' ocorre %ld vezes\n", word, total);
  free(args); free(th); free(buf);
  return 0;
}
