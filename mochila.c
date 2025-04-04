#include <stdio.h>
#include <stdlib.h>


int max(int a,int b){
    if(a>b){
        return a;
    } else {
        return b;
    }
}


int maxMochila(int it,int W,int itens[][2]){
    static int **matriz = NULL;

    // Aloca memória apenas na primeira chamada,ou seja,quando ela for NULL
    if (matriz == NULL) {
        //construção das linhas da matriz com a quantidade de itens +1,pois a primeira linha da matriz vai ser manter 0,pois não existe item 0
        matriz = (int **)malloc((it + 1) * sizeof(int *));
        for (int i = 0; i <= it; i++) {
            //o mesmo é feito com as colunas,pois a primeira coluna da matriz vai ser manter 0,já que não existe item com peso 0
            matriz[i] = (int *)malloc((W + 1) * sizeof(int));
        }
    }

    for(int i = 0; i <= it; i++){
        for(int w = 0; w <= W; w++){
            //"caso base",preenche a primeira linha e a primeira coluna da matriz com 0
            if(i==0 || w==0){
                matriz[i][w]=0;
                //Acesso a matriz de itens na linha 0(i -1,já que aqui o i=1) na coluna de índice 1,ou seja,acesso o peso na matriz de itens
                //Se o peso do item atual for maior que a capacidade(w), o item não é incluído
            } else if(w<itens[i-1][1]){
                //a matriz naquele índice recebe o valor da matriz na linha anterior na coluna de índice w
                //EM PRIMEIRO CASO,RECEBERIA O 0
                matriz[i][w] = matriz[i-1][w];
                //ùltima verificação: quando a capacidade(w) é menor ou igual ao peso do item atual(buscado na matriz de itens na linha i-1 na coluna 1)
            } else {
//aqui é realizada a comparação de quem é maior,o valor da matriz na linha anterior na coluna de índice w
//ou o valor da matriz na linha anterior na coluna de índice w menos o peso do item somado ao valor do item atual
                //escolhe o valor máximo entre incluir ou não o item
                matriz[i][w] = max(matriz[i - 1][w],matriz[i - 1][w - itens[i - 1][1]] + itens[i - 1][0]);
            }


        }
    }        

//retornará o máximo valor que pode ser colocado na mochila
    return matriz[it][W];

}




int main(){

    int W = 5;
    int itens[][2] = {
        //coluna 1: valor dos itens
        //coluna 2: peso dos itens
        {12,2},
        {10,1},
        {20,3},
        {15,2}
    };
    int it = sizeof(itens) / sizeof(itens[0]);
    int mochila = maxMochila(it,W,itens);
    printf("valor maximo da mochila: %d\n",mochila);


    return 0;
}