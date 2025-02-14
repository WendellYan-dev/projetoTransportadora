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
    int indiceOriginal;  
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
    if(a>b){
        return a;
    } else {
        return b;
    }
}

// Função de comparação para ordenar os pacotes pelo índice original
int compararIndiceOriginal(const void *a, const void *b) {
    Pacote *p1 = (Pacote *)a;
    Pacote *p2 = (Pacote *)b;
    return p1->indiceOriginal - p2->indiceOriginal;
}

// Função para trocar dois elementos
void swap(void *a, void *b, size_t size) {
    char temp[size];
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

// Função de partição do QuickSort
int partition(void *base, int low, int high, size_t size, int (*compar)(const void *, const void *)) {
    char *pivot = (char *)base + high * size;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (compar((char *)base + j * size, pivot) <= 0) {
            i++;
            swap((char *)base + i * size, (char *)base + j * size, size);
        }
    }
    swap((char *)base + (i + 1) * size, pivot, size);
    return i + 1;
}

// Função QuickSort recursiva
void quickSort(void *base, int low, int high, size_t size, int (*compar)(const void *, const void *)) {
    if (low < high) {
        int pi = partition(base, low, high, size, compar);
        quickSort(base, low, pi - 1, size, compar);
        quickSort(base, pi + 1, high, size, compar);
    }
}

// Função wrapper para o QuickSort, similar ao qsort
void meuQuickSort(void *base, size_t num, size_t size, int (*compar)(const void *, const void *)) {
    quickSort(base, 0, num - 1, size, compar);
}

void selecionarMelhoresPacotes(Veiculo* veiculo, Pacote *pacotes, int numPacotes, int pesoMax, int volumeMax, Pacote *pacotesSelecionados, int *numSelecionados, float *valorTotal) {
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
            veiculo->pesoAcumulado += pacotes[i - 1].peso;
            veiculo->volumeAcumulado += pacotes[i - 1].volume;
        }
    }

    porcentagemPeso = round((float)(veiculo->pesoAcumulado * 100) / pesoMax);
    porcentagemVolume = round((float)(veiculo->volumeAcumulado * 100) / volumeMax);

    for (int i = 0; i <= numPacotes; i++) {
        for (int w = 0; w <= pesoMax; w++) {
            free(matriz[i][w]);
        }
        free(matriz[i]);
    }
    free(matriz);
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
        printf("ERRO: Não foi possível abrir os arquivos.\n");
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
        pacotes[i].indiceOriginal = i;
        pacotes[i].alocado = 0;
    }

    for (int i = 0; i < numVeiculos; i++) {
        Pacote pacotesSelecionados[numPacotes];
        int numSelecionados = 0;
        float valorTotal = 0;

        selecionarMelhoresPacotes(&veiculos[i], pacotes, numPacotes, veiculos[i].pesoMax, veiculos[i].volumeMax, pacotesSelecionados, &numSelecionados, &valorTotal);

        // Usando o QuickSort implementado para ordenar os pacotes selecionados
        meuQuickSort(pacotesSelecionados, numSelecionados, sizeof(Pacote), compararIndiceOriginal);

        fprintf(arquivoSaida, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)->", veiculos[i].placa, valorTotal, veiculos[i].pesoAcumulado, porcentagemPeso, veiculos[i].volumeAcumulado, porcentagemVolume);
        for (int k = 0; k < numSelecionados; k++) {
            if (k > 0) {
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