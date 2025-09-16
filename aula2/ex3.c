/* 
    Implemente um algoritmo paralelo para realizar a soma de todos os elementos de um vetor de inteiros

    Não utilize variáveis compartilhadas com as threads. Cada thread deverá ter sua própria variável
    de acumulação e somente após o termino das threads é que o valor final da soma deverá ser retornato
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define TAM 10 // Tamanho do vetor
#define NUM_THREADS 4 // Número de threads

// Estrutura para passar argumentos para as threads
typedef struct {
    int* vetor;
    int inicio;
    int fim;
    int soma_parcial; // Variável de acumulação local
} Args;

// Função que será executada pelas threads
void* soma_parcial(void* arg) {
    Args* args = (Args*)arg;
    args->soma_parcial = 0; // Inicializa a soma parcial
    for (int i = args->inicio; i < args->fim; i++) {
        args->soma_parcial += args->vetor[i];
    }
    return NULL;
}

int main() {
    int vetor[TAM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    pthread_t threads[NUM_THREADS];
    Args args[NUM_THREADS];
    int soma_total = 0;

    int intervalo = TAM / NUM_THREADS;

    // Criar as threads
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].vetor = vetor;
        args[i].inicio = i * intervalo;
        args[i].fim = (i == NUM_THREADS - 1) ? TAM : (i + 1) * intervalo;
        args[i].soma_parcial = 0;

        if (pthread_create(&threads[i], NULL, soma_parcial, &args[i]) != 0) {
            perror("Erro ao criar thread");
            return 1;
        }
    }

    // Fazer join em todas as threads e acumular os resultados
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Erro ao fazer join na thread");
            return 1;
        }
        soma_total += args[i].soma_parcial; // Acumula a soma parcial de cada thread
    }

    // Imprimir o resultado final
    printf("Soma total: %d\n", soma_total);

    return 0;
}