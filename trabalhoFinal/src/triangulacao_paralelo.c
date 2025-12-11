#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "libppc.h"

void gaussian_elimination_parallel(double *A, long int n, int num_threads) {
    for (long int k = 0; k < n - 1; k++) {
        double pivot = M(k, k, n, A);
        if (pivot == 0.0) {
            fprintf(stderr, "Aviso: pivô zero na linha %ld (sem pivotamento)\n", k);
        }

        // DEPENDÊNCIA DE DADOS ENTRE ITERAÇÕES:
        // O loop externo em k NÃO pode ser paralelizado:
        // a iteração k+1 depende dos elementos atualizados na iteração k
        // Ou seja, existe dependência de dados entre iterações de k

        // Por isso, paralelizamos apenas o loop nas linhas i (abaixo do pivô),
        // mantendo a ordem sequencial no loop de k
        #pragma omp parallel for num_threads(num_threads) default(none) shared(A, n, k, pivot)
        for (long int i = k + 1; i < n; i++) {
            double factor = (pivot != 0.0) ? M(i, k, n, A) / pivot : 0.0;

            // Cada thread atualiza elementos da linha i, que é exclusiva daquela iteração de i
            // Não há dependência entre iterações de i dentro do MESMO valor de k, pois
            // cada thread só mexe na sua própria linha
            for (long int j = k; j < n; j++) {
                M(i, j, n, A) -= factor * M(k, j, n, A);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Uso: %s <N> <arquivo_matriz> [num_threads]\n", argv[0]);
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    const char *file_mat = argv[2];
    int num_threads = (argc == 4) ? atoi(argv[3]) : 4;

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    if (num_threads <= 0) num_threads = 4;

    double *A = load_double_matrix(file_mat, n, n);
    if (!A) {
        fprintf(stderr, "Erro ao carregar matriz do arquivo %s\n", file_mat);
        return EXIT_FAILURE;
    }

    double start = omp_get_wtime();
    gaussian_elimination_parallel(A, n, num_threads);
    double end = omp_get_wtime();

    if (save_double_matrix(A, n, n, "saida.out") != 0) {
        fprintf(stderr, "Erro ao salvar matriz em saida.out\n");
    }

    printf("Threads: %d\n", num_threads);
    printf("Tempo paralelo (s): %f\n", end - start);

    free(A);
    return EXIT_SUCCESS;
}
