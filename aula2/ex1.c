/* Crie um algoritmo que crie uma thread para imprimir cada letra do alfabeto 
    Seu programa deverá receber a letra a ser impressa por meio de um parâmetro
    para a thread em questão. Para isso crie um vetor contendo as letras do alfabeto
    e passe como parâmetro da thread a posição ou dado desse vetor
    
    Não é necessário imprimir em ordem
    Lembre-se de fazer um join após criar as threads

*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_LETRAS 26

void* imprime_letra(void* arg) {
    //pega o valor do endereço que foi passado pra thread
    char letra = *(char*)arg;
    printf("%c\n", letra);
    return NULL;
}

int main() {
    pthread_t threads[NUM_LETRAS];
    char alfabeto[NUM_LETRAS] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // cria uma thread pra cada letra
    for (int i = 0; i < NUM_LETRAS; i++) {
        //cria um ponteiro pra a posição do vetor
        char* letra = &alfabeto[i];
        if (pthread_create(&threads[i], NULL, imprime_letra, letra) != 0) {
            perror("erro pra criar thread");
            return 1;
        }
    }

    // faz join em todas as threads
    for (int i = 0; i < NUM_LETRAS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("erro pra fazer join na thread");
            return 1;
        }
    }

    return 0;
}