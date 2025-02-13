#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


int max(int a,int b){
    if(a>b){
        return a;
    } else {
        return b;
    }
}


int maxMochila(int it,int W,int V,int itens[][3],bool usados[]){
    static int ***matriz = NULL;

    // Aloca memória apenas na primeira chamada,ou seja,quando ela for NULL
    if (matriz == NULL) {
        //construção das linhas da matriz com a quantidade de itens +1,pois a primeira linha da matriz vai ser manter 0,pois não existe item 0
        matriz = (int ***)malloc((it + 1) * sizeof(int **));
        for (int i = 0; i <= it; i++) {
            //o mesmo é feito com as colunas,pois a primeira coluna da matriz vai ser manter 0,já que não existe item com peso 0
            matriz[i] = (int **)malloc((W + 1) * sizeof(int *));
            for (int j = 0; j <= W; j++) {
                matriz[i][j] = (int *)malloc((V+1) * sizeof(int));
            }    
        }
    }

    for(int i = 0; i <= it; i++){
        for(int w = 0; w <= W; w++){
            for(int v = 0; v <= V; v++){

                //"caso base",preenche a primeira linha,a primeira coluna e a primeira célula da matriz com 0
                if(i==0 || w==0 || v==0){
                    matriz[i][w][v]=0;
                    //Acesso a matriz de itens na linha 0(i -1,já que aqui o i=1) na coluna de índice 1,ou seja,acesso o peso na matriz de itens
                    //Se o peso do item atual for maior que a capacidade(w), o item não é incluído
                } else if(w<itens[i-1][1] || v<itens[i-1][2]){
                    //a matriz naquele índice recebe o valor da matriz na linha anterior na coluna de índice w
                    //EM PRIMEIRO CASO,RECEBERIA O 0
                    matriz[i][w][v] = matriz[i-1][w][v];
                    //ùltima verificação: quando a capacidade(w) é menor ou igual ao peso do item atual(buscado na matriz de itens na linha i-1 na coluna 1)
                } else {
    //aqui é realizada a comparação de quem é maior,o valor da matriz na linha anterior na coluna de índice w
    //ou o valor da matriz na linha anterior na coluna de índice w menos o peso do item somado ao valor do item atual
                    //escolhe o valor máximo entre incluir ou não o item
                    matriz[i][w][v] = max(matriz[i - 1][w][v],matriz[i - 1][w - itens[i - 1][1]][v - itens[i - 1][2]] + itens[i - 1][0]);
                }



            }


        }
    }        
    int maxMatriz = matriz[it][W][V];
    
    //caminho de volta para sabermos se o item foi utilizado ou não
    int w = W,v = V;
    //começarei do final da ponta final da matriz e perccorro ela de trás p frente
    //se o valor da célula atual for diferente ao valor da célula anterior,significa q ele foi utilizado
    
    for(int i = it; i > 0; i--){
        if(matriz[i][w][v]!= matriz[i-1][w][v]){
            //atribuímos esse elemento ao índice do vetor usados e valor false
            usados[i-1]= true;
            //como o item foi utilizado,devemos pular a quantidade de colunas(peso) e quantidade de profundidade(volume) do item
            w = w - itens[i-1][1];
            v = v - itens[i-1][2];  
        }    
        
    }
    
        //retornará o máximo valor que pode ser colocado na mochila
        return maxMatriz;

}




int main(){

    int V = 20;
    int W = 5;
    int itens[4][3] = {
        //coluna 1: valor dos itens
        //coluna 2: peso dos itens
        //coluna 3: volume dos itens
        {12,2,12},
        {10,1,6},
        {20,3,14},
        {15,2,8}
    };
    int it = sizeof(itens) / sizeof(itens[0]);
    bool usados[it];
    for(int i = 0; i < it; i++){
      usados[i] = false;
    }   
    int mochila = maxMochila(it,W,V,itens,usados);


    printf("valor maximo da mochila: %d\n",mochila);

    printf("Itens utilizados:\n");
    for (int i = 0; i < it; i++) {
        if (usados[i]) {
            printf("- Item %d: Valor = %d, Peso = %d, Volume = %d\n", 
                   i + 1, itens[i][0], itens[i][1], itens[i][2]);
        }
    }

    printf("Itens nao utilizados:\n");
    for (int i = 0; i < it; i++) {
        if (!usados[i]) {
            printf("- Item %d: Valor = %d, Peso = %d, Volume = %d\n", 
                   i + 1, itens[i][0], itens[i][1], itens[i][2]);
        }
    }


    return 0;
}