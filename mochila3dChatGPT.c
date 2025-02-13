#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int max(int a, int b) {
    return (a > b) ? a : b;
}

int maxMochila(int it, int W, int V, int itens[][3], bool usados[]) {
    static int ***matriz = NULL;  // Matriz estática para reutilização
    static int prev_it = 0, prev_W = 0, prev_V = 0;  // Parâmetros anteriores para verificar realocações

    // Realoca a matriz apenas se os tamanhos mudarem
    if (matriz == NULL || prev_it != it || prev_W != W || prev_V != V) {
        // Libera a matriz anterior se ela já foi alocada
        if (matriz != NULL) {
            for (int i = 0; i <= prev_it; i++) {
                for (int j = 0; j <= prev_W; j++) {
                    free(matriz[i][j]);
                }
                free(matriz[i]);
            }
            free(matriz);
        }

        // Aloca nova matriz para os tamanhos atuais
        matriz = (int ***)malloc((it + 1) * sizeof(int **));
        for (int i = 0; i <= it; i++) {
            matriz[i] = (int **)malloc((W + 1) * sizeof(int *));
            for (int j = 0; j <= W; j++) {
                matriz[i][j] = (int *)malloc((V + 1) * sizeof(int));
            }
        }

        // Atualiza os parâmetros para o próximo uso
        prev_it = it;
        prev_W = W;
        prev_V = V;
    }

    // Preenche a matriz usando Programação Dinâmica
    for (int i = 0; i <= it; i++) {
        for (int w = 0; w <= W; w++) {
            for (int v = 0; v <= V; v++) {
                if (i == 0 || w == 0 || v == 0) {
                    matriz[i][w][v] = 0;  // Caso base
                } else if (w < itens[i - 1][1] || v < itens[i - 1][2]) {
                    matriz[i][w][v] = matriz[i - 1][w][v];  // Item não incluído
                } else {
                    matriz[i][w][v] = max(matriz[i - 1][w][v],
                                         matriz[i - 1][w - itens[i - 1][1]][v - itens[i - 1][2]] + itens[i - 1][0]);
                }
            }
        }
    }

    int maxMatriz = matriz[it][W][V];

    // Determina os itens usados
    int w = W, v = V;
    for (int i = it; i > 0; i--) {
        if (matriz[i][w][v] != matriz[i - 1][w][v]) {
            usados[i - 1] = true;
            w -= itens[i - 1][1];
            v -= itens[i - 1][2];
        } else {
            usados[i - 1] = false;
        }
    }

    return maxMatriz;
}

int main() {
    int itens[4][3] = {
        {12, 2, 12},  // {valor, peso, volume}
        {10, 1, 6},
        {20, 3, 14},
        {15, 2, 8}
    };
    int it = sizeof(itens) / sizeof(itens[0]);

    // Teste com múltiplas chamadas da função maxMochila
    bool usados[it];

    // Primeira chamada
    int mochila1 = maxMochila(it, 5, 20, itens, usados);
    printf("Primeira chamada - Valor máximo: %d\n", mochila1);
    printf("Itens utilizados:\n");
    for (int i = 0; i < it; i++) {
        if (usados[i]) {
            printf("- Item %d: Valor = %d, Peso = %d, Volume = %d\n", i + 1, itens[i][0], itens[i][1], itens[i][2]);
        }
    }

    // Segunda chamada com capacidade menor
    int mochila2 = maxMochila(it, 4, 15, itens, usados);
    printf("\nSegunda chamada - Valor máximo: %d\n", mochila2);
    printf("Itens utilizados:\n");
    for (int i = 0; i < it; i++) {
        if (usados[i]) {
            printf("- Item %d: Valor = %d, Peso = %d, Volume = %d\n", i + 1, itens[i][0], itens[i][1], itens[i][2]);
        }
    }

    // Terceira chamada com capacidade maior
    int mochila3 = maxMochila(it, 6, 25, itens, usados);
    printf("\nTerceira chamada - Valor máximo: %d\n", mochila3);
    printf("Itens utilizados:\n");
    for (int i = 0; i < it; i++) {
        if (usados[i]) {
            printf("- Item %d: Valor = %d, Peso = %d, Volume = %d\n", i + 1, itens[i][0], itens[i][1], itens[i][2]);
        }
    }

    return 0;
}
