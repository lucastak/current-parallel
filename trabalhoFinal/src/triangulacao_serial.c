#include <stdio.h>
#include <stdlib.h>
#include <libppc.h>

void gaussian_elimination_serial(double *A, long int n) {
    for (long int k = 0; k < n - 1; k++) {
        double pivot = M(k, k, n, A);
        if (pivot == 0.0) {
            fprintf(stderr, "Aviso: pivô zero na linha %ld (sem pivotamento)\n", k);
        }

        for (long int i = k + 1; i < n; i++) {
            double factor = (pivot != 0.0) ? M(i, k, n, A) / pivot : 0.0;

            for (long int j = k; j < n; j++) {
                M(i, j, n, A) -= factor * M(k, j, n, A);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <N> <arquivo_matriz>\n", argv[0]);
        return EXIT_FAILURE;
    }

    long int n = atol(argv[1]);
    const char *file_mat = argv[2];

    if (n <= 0) {
        fprintf(stderr, "N deve ser > 0\n");
        return EXIT_FAILURE;
    }

    double *A = load_double_matrix(file_mat, n, n);
    if (!A) {
        fprintf(stderr, "Erro ao carregar matriz do arquivo %s\n", file_mat);
        return EXIT_FAILURE;
    }

    gaussian_elimination_serial(A, n);

    if (save_double_matrix(A, n, n, "saida.out") != 0) {
        fprintf(stderr, "Erro ao salvar matriz em saida.out\n");
    }

    free(A);
    return EXIT_SUCCESS;
}
