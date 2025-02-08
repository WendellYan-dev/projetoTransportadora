#include <stdio.h>
#include <stdlib.h>


int fibonacci(int n){
    static int *vetor = NULL;
    if (vetor == NULL) {
        vetor = (int *)malloc((n+1)*sizeof(int));
    }
    if(n<=1){
        return n;
    }
    vetor[0] = 0,vetor[1] = 1;

    for(int i = 2;i<=n;i++){
        vetor[i] = vetor[i-1] + vetor[i-2];
    }
    return vetor[n];
}

int main() {
    //quebra o código em 49
    int num = 49;
    printf("fibonacci de %d e %d\n", num, fibonacci(num));



    return 0;
}


int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int knapsack(int W, int wt[], int val[], int n) {
    int i, w;
    static int **K = NULL; // Declaração estática

    // Alocar memória apenas na primeira chamada
    if (K == NULL) {
        K = (int **)malloc((n + 1) * sizeof(int *));
        for (i = 0; i <= n; i++) {
            K[i] = (int *)malloc((W + 1) * sizeof(int));
        }
    }

    // Preencher a tabela K[][]
    for (i = 0; i <= n; i++) {
        for (w = 0; w <= W; w++) {
            if (i == 0 || w == 0)
                K[i][w] = 0;
            else if (wt[i - 1] <= w)
                K[i][w] = max(val[i - 1] + K[i - 1][w - wt[i - 1]], K[i - 1][w]);
            else
                K[i][w] = K[i - 1][w];
        }
    }

    // Retornar o resultado
    return K[n][W];
}