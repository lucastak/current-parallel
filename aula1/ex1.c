#include <stdio.h>

int main() {
    int max = 10;
    int processador = 2;
    int no;
    int my_min, my_max;

    for (int i = 0; i < processador; i++) {
        my_min = (i*max)/processador;
        my_max = ((i+1)*max)/processador-1;
        printf("Processador %d: de %d até %d\n", i, my_min, my_max);

        // A estratégia usada foi fazer o balanceamento de carga entre os processadores, dividindo o trabalho de forma equitativa.
        // o mínimo é o valor do processador vezes o máximo dividido pelo número de processadores.
        // o máximo é o valor do processador mais 1 vezes o máximo dividido pelo número de processadores, menos 1.
    }
    return 0;
}
