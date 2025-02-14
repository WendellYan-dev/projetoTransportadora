#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


int porcentagemPeso =0,porcentagemVolume = 0;

// Estrutura para representar um pacote
typedef struct {
    char codigo[14];       // Código do pacote
    float valor;           // Valor monetário do pacote
    int peso;              // Peso do pacote em KG
    int volume;            // Volume do pacote em Litros
    int indiceOriginal;    // Índice original do pacote na lista de entrada
    int alocado;           // Flag para indicar se o pacote já foi alocado a um veículo
} Pacote;

// Estrutura para representar um veículo
typedef struct {
    char placa[8];         // Placa do veículo
    int pesoMax;           // Capacidade máxima de peso do veículo
    int volumeMax; 
    int pesoAcumulado;
    int volumeAcumulado;        // Capacidade máxima de volume do veículo
} Veiculo;

// Função que retorna o maior valor entre dois números
float max(float a, float b) {
    if(a>b){
        return a;
    } else {
        return b;
    }
}

// Função de comparação para ordenar pacotes por índice original
int compararPorIndiceOriginal(const void *a, const void *b) {
    return ((Pacote *)a)->indiceOriginal - ((Pacote *)b)->indiceOriginal;
}

// Função para encontrar a melhor combinação de pacotes usando programação dinâmica
void selecionarMelhoresPacotes(Veiculo* veiculo,Pacote *pacotes, int numPacotes, int pesoMax, int volumeMax, Pacote *pacotesSelecionados, int *numSelecionados, float *valorTotal) {
    float ***matriz = (float ***)malloc((numPacotes + 1) * sizeof(float **));
    for (int i = 0; i <= numPacotes; i++) {
        matriz[i] = (float **)malloc((pesoMax + 1) * sizeof(float *));
        for (int w = 0; w <= pesoMax; w++) {
            matriz[i][w] = (float *)calloc((volumeMax + 1), sizeof(float));
        }
    }

    for (int i = 1; i <= numPacotes; i++) {
        Pacote pacoteAtual = pacotes[i - 1];
        for (int w = 0; w <= pesoMax; w++) {
            for (int v = 0; v <= volumeMax; v++) {
                if (pacoteAtual.peso <= w && pacoteAtual.volume <= v && !pacoteAtual.alocado) {
                    matriz[i][w][v] = max(matriz[i - 1][w][v], matriz[i - 1][w - pacoteAtual.peso][v - pacoteAtual.volume] + pacoteAtual.valor);
                } else {
                    matriz[i][w][v] = matriz[i - 1][w][v];
                }
            }
        }
    }

    *valorTotal = matriz[numPacotes][pesoMax][volumeMax];
    *numSelecionados = 0;

    int pesoRestante = pesoMax, volumeRestante = volumeMax;
    for (int i = numPacotes; i > 0 && pesoRestante > 0 && volumeRestante > 0; i--) {
        if (matriz[i][pesoRestante][volumeRestante] != matriz[i - 1][pesoRestante][volumeRestante]) {
            pacotesSelecionados[(*numSelecionados)++] = pacotes[i - 1];
            pacotes[i - 1].alocado = 1;
            pesoRestante -= pacotes[i - 1].peso;
            volumeRestante -= pacotes[i - 1].volume;
            veiculo->pesoAcumulado+= pacotes[i-1].peso;
            veiculo->volumeAcumulado+= pacotes[i-1].volume;
        }
    }

    porcentagemPeso = round((float)(veiculo->pesoAcumulado*100)/pesoMax);
    porcentagemVolume = round((float)(veiculo->volumeAcumulado*100)/volumeMax);

    for (int i = 0; i <= numPacotes; i++) {
        for (int w = 0; w <= pesoMax; w++) {
            free(matriz[i][w]);
        }
        free(matriz[i]);
    }
    free(matriz);
}

// Função para imprimir pacotes não alocados
void imprimirPacotesPendentes(Pacote *pacotes, int numPacotes, FILE *arquivoSaida) {
    fprintf(arquivoSaida, "PENDENTE:");
    int primeiroPacote = 1;
    float valorPendentes= 0;
    int pesoPendentes = 0;
    int volumePendentes =0;
    for (int i = 0; i < numPacotes; i++) {
        if (!pacotes[i].alocado) {
            valorPendentes+= pacotes[i].valor;
            pesoPendentes+= pacotes[i].peso;
            volumePendentes+= pacotes[i].volume;
        }
    }
    fprintf(arquivoSaida,"R$%.2f,%dKG,%dL->",valorPendentes,pesoPendentes,volumePendentes);
    for(int j = 0;j < numPacotes;j++){
        if(!pacotes[j].alocado){
            if(!primeiroPacote){
                fprintf(arquivoSaida, ",");
            }
            fprintf(arquivoSaida, "%s", pacotes[j].codigo);
        }
        primeiroPacote = 0;
    }
    fprintf(arquivoSaida, "\n");
}

int main(int argc, char *argv[]) {
    FILE* arquivoEntrada = fopen(argv[1], "r");
    FILE* arquivoSaida = fopen(argv[2], "w");
    if (!arquivoEntrada || !arquivoSaida) {
        printf("ERRO: Não foi possível abrir os arquivos.\n");
        return 1;
    }

    int numVeiculos;
    fscanf(arquivoEntrada, "%d", &numVeiculos);
    Veiculo *veiculos = malloc(numVeiculos * sizeof(Veiculo));
    for (int i = 0; i < numVeiculos; i++) {
        veiculos[i].pesoAcumulado = 0;
        veiculos[i].volumeAcumulado=0;
        fscanf(arquivoEntrada, "%s %d %d", veiculos[i].placa, &veiculos[i].pesoMax, &veiculos[i].volumeMax);
    }

    int numPacotes;
    fscanf(arquivoEntrada, "%d", &numPacotes);
    Pacote *pacotes = malloc(numPacotes * sizeof(Pacote));
    for (int i = 0; i < numPacotes; i++) {
        fscanf(arquivoEntrada, "%s %f %d %d", pacotes[i].codigo, &pacotes[i].valor, &pacotes[i].peso, &pacotes[i].volume);
        pacotes[i].indiceOriginal = i;
        pacotes[i].alocado = 0;
    }

    for (int i = 0; i < numVeiculos; i++) {
        Pacote pacotesSelecionados[numPacotes];
        int numSelecionados = 0;
        float valorTotal = 0;

        selecionarMelhoresPacotes(&veiculos[i],pacotes, numPacotes, veiculos[i].pesoMax, veiculos[i].volumeMax, pacotesSelecionados, &numSelecionados, &valorTotal);

        qsort(pacotesSelecionados, numSelecionados, sizeof(Pacote), compararPorIndiceOriginal);


        // Escreve o resultado diretamente no arquivo de saída
        fprintf(arquivoSaida, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)->", veiculos[i].placa, valorTotal,veiculos[i].pesoAcumulado ,porcentagemPeso,veiculos[i].volumeAcumulado,porcentagemVolume);
        for (int k = 0; k < numSelecionados; k++) {
            if (k > 0){
                fprintf(arquivoSaida, ",");
            } 
            fprintf(arquivoSaida, "%s", pacotesSelecionados[k].codigo);
        }
        fprintf(arquivoSaida, "\n");
    }

    imprimirPacotesPendentes(pacotes, numPacotes, arquivoSaida);

    free(veiculos);
    free(pacotes);
    fclose(arquivoEntrada);
    fclose(arquivoSaida);
    return 0;
}
