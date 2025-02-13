#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char placa[8];
    int peso;
    int capacidade;
}Veiculo;

typedef struct {
    char codigo[14];
    float preco;
    int p;
    int volume;
}Pacote;







int main(int argc, char *argv[]) {
    FILE* arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        printf("ERRO\n");
        return 0;
    }
    FILE* saida = fopen(argv[2], "w");
    if (saida == NULL) {
        printf("ERRO\n");
        return 0;
    }

    int numeroVeiculos, numeroPacotes;
    

    fscanf(arquivo,"%d", &numeroVeiculos);
    Veiculo* veiculo = (Veiculo*) malloc(numeroVeiculos * sizeof(Veiculo));
    if (veiculo == NULL) {
        printf("ERRO\n");
        return 0;
    }
    for(int i = 0; i < numeroVeiculos; i++){
        fscanf(arquivo,"%s %d %d",veiculo[i].placa,&veiculo[i].peso,&veiculo[i].capacidade);
    }

    fscanf(arquivo,"%d", &numeroPacotes);
    Pacote* pacote = (Pacote*) malloc(numeroPacotes * sizeof(Pacote));
    if (pacote == NULL) {
        printf("ERRO\n");
        return 0;
    }

    for(int j =0;j<numeroPacotes;j++){
        fscanf(arquivo,"%s %f %d %d",pacote[j].codigo,&pacote[j].preco,&pacote[j].p,&pacote[j].volume);
    }
    
    //saída no arquivo output
    for(int i = 0; i < numeroVeiculos; i++){
        fprintf(saida,"%s %d %d",veiculo[i].placa,veiculo[i].peso,veiculo[i].capacidade);
        fprintf(saida,"\n");
    }
    for(int j =0;j<numeroPacotes;j++){
        fprintf(saida,"%s %f %d %d",pacote[j].codigo,pacote[j].preco,pacote[j].p,pacote[j].volume);
        fprintf(saida,"\n");
    }
    

    fclose(arquivo);
    fclose(saida);

    return 0;
}