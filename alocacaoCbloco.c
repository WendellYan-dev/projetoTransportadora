#include <stdio.h>
#include <stdlib.h>

int ***alocarMatriz(int X, int Y, int Z) {
    // 1. Aloca um único bloco contíguo para armazenar os dados
    int *data = (int *)malloc(X * Y * Z * sizeof(int));

    // 2. Aloca ponteiros para as linhas da matriz
    int ***M = (int ***)malloc(X * sizeof(int **));
    for (int i = 0; i < X; i++) {
        M[i] = (int **)malloc(Y * sizeof(int *));
        for (int j = 0; j < Y; j++) {
            M[i][j] = data + (i * Y * Z) + (j * Z); // Aponta para a posição correta
        }
    }

    return M; // Retorna a matriz com acesso normal via M[i][j][k]
}

void liberarMatriz(int ***M, int X) {
    free(M[0][0]); // Libera o bloco de dados contíguo
    for (int i = 0; i < X; i++)
        free(M[i]); // Libera os ponteiros das linhas
    free(M); // Libera o array de ponteiros de primeira dimensão
}

int main() {
    int X = 3, Y = 3, Z = 3;
    int ***M = alocarMatriz(X, Y, Z);

    // Teste: preencher a matriz
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++)
            for (int k = 0; k < Z; k++)
                M[i][j][k] = i * 100 + j * 10 + k;

    // Exibir a matriz
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++)
                printf("%3d ", M[i][j][k]);
            printf(" | ");
        }
        printf("\n");
    }

    liberarMatriz(M, X);
    return 0;
}
