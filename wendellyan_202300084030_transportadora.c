#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int porcentagemPeso = 0, porcentagemVolume = 0;


typedef struct {
    char codigo[14];       
    float valor;           
    int peso;              
    int volume;              
    int alocado;           
} Pacote;


typedef struct {
    char placa[8];         
    int pesoMax;           
    int volumeMax; 
    int pesoAcumulado;
    int volumeAcumulado;   
} Veiculo;


float max(float a, float b) {
    if(a>=b){
        return a;
    } else {
        return b;
    }
}

void selecionarMelhoresPacotes(Veiculo* veiculo, Pacote *pacotes, int numPacotes, int pesoMax, int volumeMax, Pacote *pacotesSelecionados, int *contSelecionados, float *valorTotal) {
    float *alocacao = (float *)calloc((numPacotes + 1) * (pesoMax + 1) * (volumeMax + 1), sizeof(float));
    if (alocacao == NULL) {
        printf("Erro de alocação de memória\n");
    }

    
    float ***matriz = (float ***)malloc((numPacotes + 1) * sizeof(float **));
    if (matriz == NULL) {
        free(alocacao);
        printf("Erro de alocação de memória\n");
    }

    for (int i = 0; i <= numPacotes; i++) {
        matriz[i] = (float **)malloc((pesoMax + 1) * sizeof(float *));
        if (matriz[i] == NULL) {
            
            for (int j = 0; j < i; j++) free(matriz[j]);
            free(matriz);
            free(alocacao);
            printf("Erro de alocação de memória\n");
        }

        for (int w = 0; w <= pesoMax; w++) {
            matriz[i][w] = &alocacao[i * (pesoMax + 1) * (volumeMax + 1) + w * (volumeMax + 1)];
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
    *contSelecionados = 0;

    
    int pesoRestante = pesoMax, volumeRestante = volumeMax;
    for (int i = numPacotes; i > 0 && pesoRestante > 0 && volumeRestante > 0; i--) {
        if (matriz[i][pesoRestante][volumeRestante] != matriz[i - 1][pesoRestante][volumeRestante]) {
            pacotesSelecionados[(*contSelecionados)++] = pacotes[i - 1];
            pacotes[i - 1].alocado = 1;
            pesoRestante -= pacotes[i - 1].peso;
            volumeRestante -= pacotes[i - 1].volume;
            veiculo->pesoAcumulado += pacotes[i - 1].peso;
            veiculo->volumeAcumulado += pacotes[i - 1].volume;
        }
    }

    
    porcentagemPeso = round((float)(veiculo->pesoAcumulado * 100) / pesoMax);
    porcentagemVolume = round((float)(veiculo->volumeAcumulado * 100) / volumeMax);

    
    for (int i = 0; i <= numPacotes; i++) {
        free(matriz[i]);
    }
    free(matriz);
    free(alocacao);
}

void imprimirPacotesPendentes(Pacote *pacotes, int numPacotes, FILE *arquivoSaida) {
    fprintf(arquivoSaida, "PENDENTE:");
    int primeiroPacote = 1;
    float valorPendentes = 0;
    int pesoPendentes = 0;
    int volumePendentes = 0;
    for (int i = 0; i < numPacotes; i++) {
        if (!pacotes[i].alocado) {
            valorPendentes += pacotes[i].valor;
            pesoPendentes += pacotes[i].peso;
            volumePendentes += pacotes[i].volume;
        }
    }
    fprintf(arquivoSaida, "R$%.2f,%dKG,%dL->", valorPendentes, pesoPendentes, volumePendentes);
    for (int j = 0; j < numPacotes; j++) {
        if (!pacotes[j].alocado) {
            if (!primeiroPacote) {
                fprintf(arquivoSaida, ",");
            }
            fprintf(arquivoSaida, "%s", pacotes[j].codigo);
            primeiroPacote = 0;
        }
    }
    fprintf(arquivoSaida, "\n");
}

int main(int argc, char *argv[]) {
    FILE* arquivoEntrada = fopen(argv[1], "r");
    FILE* arquivoSaida = fopen(argv[2], "w");
    if (!arquivoEntrada || !arquivoSaida) {
        printf("ERRO");
        return 1;
    }

    int numVeiculos;
    fscanf(arquivoEntrada, "%d", &numVeiculos);
    Veiculo *veiculos = malloc(numVeiculos * sizeof(Veiculo));
    for (int i = 0; i < numVeiculos; i++) {
        veiculos[i].pesoAcumulado = 0;
        veiculos[i].volumeAcumulado = 0;
        fscanf(arquivoEntrada, "%s %d %d", veiculos[i].placa, &veiculos[i].pesoMax, &veiculos[i].volumeMax);
    }

    int numPacotes;
    fscanf(arquivoEntrada, "%d", &numPacotes);
    Pacote *pacotes = malloc(numPacotes * sizeof(Pacote));
    for (int i = 0; i < numPacotes; i++) {
        fscanf(arquivoEntrada, "%s %f %d %d", pacotes[i].codigo, &pacotes[i].valor, &pacotes[i].peso, &pacotes[i].volume);
        pacotes[i].alocado = 0;
    }

    for (int i = 0; i < numVeiculos; i++) {
        Pacote pacotesSelecionados[numPacotes];
        int contSelecionados = 0;
        float valorTotal = 0;

        selecionarMelhoresPacotes(&veiculos[i], pacotes, numPacotes, veiculos[i].pesoMax, veiculos[i].volumeMax, pacotesSelecionados, &contSelecionados, &valorTotal);


        fprintf(arquivoSaida, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)->", veiculos[i].placa, valorTotal, veiculos[i].pesoAcumulado, porcentagemPeso, veiculos[i].volumeAcumulado, porcentagemVolume);
        for (int k = contSelecionados-1; k >= 0; k--) {
            if (k < contSelecionados-1) {
                fprintf(arquivoSaida,",");
            }
            fprintf(arquivoSaida,"%s",pacotesSelecionados[k].codigo);
        }        
        fprintf(arquivoSaida,"\n");
    }

    imprimirPacotesPendentes(pacotes, numPacotes, arquivoSaida);

    free(veiculos);
    free(pacotes);
    fclose(arquivoEntrada);
    fclose(arquivoSaida);
    return 0;
}