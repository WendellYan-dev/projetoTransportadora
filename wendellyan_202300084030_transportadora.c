#include <stdio.h>
#include <stdlib.h>


// typedef struct {
//     int item;
//     int valor;
//     int peso;
// } Itens;    




//problema da mochila
/*
uma mochila possui n itens,cada item(i) tem um peso(w) e um valor(v)
o objetivo é maximizar o valor total da mochila sem exceder o peso maximo(W)
onde i>=1 e i<=n e w>=1 e w<=W



*/

void somaItens(int it,int w,int W){
    static int **matriz = NULL;
    matriz = (int**)malloc((it+1)*sizeof(int*));
    if(matriz == NULL){
        printf("Erro");
    }
    matriz[0] = (int *)malloc((it+1)*(W+1)* sizeof(int));
    if(matriz[0] == NULL){
        printf("Erro");
    }
    //matriz[i] precisa apontar para o início da linha i, que está i * colunas posições depois de matriz[0].
    //Se não ajustarmos os ponteiros, matriz[i] conterá lixo e matriz[i][j] levará a acesso inválido ou segmentation fault
    for (int i = 1; i < it+1; i++) {
    matriz[i] = matriz[0] + i * (W+1);
    }

    if(it ==0 || w == 0){
        for(int i = 0; i <= it; i++){
            matriz[i][0] = 0;
        }
        for(int j = 0; j <= W; j++){
            matriz[0][j] = 0;
        }
    }

    


    for(int i = 0;i<=it;i++){
        for(int j =0;j<=W;j++){
            printf("%3d ", matriz[i][j]);  

        }
        printf("\n");
    }

}




int main() {

    /*
    matriz 3 por 4
    linha[0]=itens
    linha[1]=valores
    linha[2]=pesos
*/
    // int itens [3][4] = {
    //     {1, 2, 3 , 4},
    //     {12, 10, 20, 15},
    //     {2, 1, 3 , 2}
    // }

    printf("MATRIZ INICIADA\n");
    somaItens(4,0,5);



    return 0;
}