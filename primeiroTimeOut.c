#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Estrutura para representar um pacote
typedef struct {
    char codigo[14];
    float valor;
    int peso;
    int volume;
    int indiceOriginal;
    int alocado; // Indica se o pacote já foi alocado
} Pacote;

// Estrutura para representar um veículo
typedef struct {
    char placa[8];
    int pesoMax;
    int volumeMax;
} Veiculo;

// Função de comparação para ordenar por índice original
int comparar_por_indice_original(const void *a, const void *b) {
    Pacote *p1 = (Pacote *)a;
    Pacote *p2 = (Pacote *)b;
    return p1->indiceOriginal - p2->indiceOriginal;
}

// Função para encontrar a melhor combinação de pacotes usando programação dinâmica
void mochila_3d(Pacote *pacotes, int m, int pesoMax, int volumeMax, Pacote *selecionados, int *count, float *valor_total) {
    // Alocação da matriz DP
    float ***dp = (float ***)malloc((m + 1) * sizeof(float **));
    for (int i = 0; i <= m; i++) {
        dp[i] = (float **)malloc((pesoMax + 1) * sizeof(float *));
        for (int w = 0; w <= pesoMax; w++) {
            dp[i][w] = (float *)calloc((volumeMax + 1), sizeof(float));
        }
    }

    // Preenchimento da matriz DP
    for (int i = 1; i <= m; i++) {
        Pacote p = pacotes[i - 1];
        for (int w = 0; w <= pesoMax; w++) {
            for (int v = 0; v <= volumeMax; v++) {
                if (p.peso <= w && p.volume <= v && !p.alocado) {
                    dp[i][w][v] = fmax(dp[i - 1][w][v], dp[i - 1][w - p.peso][v - p.volume] + p.valor);
                } else {
                    dp[i][w][v] = dp[i - 1][w][v];
                }
            }
        }
    }

    // Reconstrução da solução
    *valor_total = dp[m][pesoMax][volumeMax];
    *count = 0;
    int w = pesoMax, v = volumeMax;
    for (int i = m; i > 0 && w > 0 && v > 0; i--) {
        if (dp[i][w][v] != dp[i - 1][w][v]) {
            selecionados[(*count)++] = pacotes[i - 1];
            pacotes[i - 1].alocado = 1; // Marca o pacote como alocado
            w -= pacotes[i - 1].peso;
            v -= pacotes[i - 1].volume;
        }
    }

    // Liberação da memória da matriz DP
    for (int i = 0; i <= m; i++) {
        for (int w = 0; w <= pesoMax; w++) {
            free(dp[i][w]);
        }
        free(dp[i]);
    }
    free(dp);
}


// Função para imprimir pacotes não alocados
void imprimir_pendentes(Pacote *pacotes, int m, FILE *saida) {
    fprintf(saida, "PENDENTE:");
    int primeiro = 1;
    for (int i = 0; i < m; i++) {
        if (!pacotes[i].alocado) {
            if (!primeiro) fprintf(saida, ",");
            fprintf(saida, "R$%.2f,%dKG,%dL->%s", pacotes[i].valor, pacotes[i].peso, pacotes[i].volume, pacotes[i].codigo);
            primeiro = 0;
        }
    }
    fprintf(saida, "\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE* arquivo = fopen(argv[1], "r");
    FILE* saida = fopen(argv[2], "w");
    if (!arquivo || !saida) {
        printf("ERRO: Não foi possível abrir os arquivos.\n");
        return 1;
    }
    
    int n, m;
    fscanf(arquivo, "%d", &n);
    Veiculo *veiculos = malloc(n * sizeof(Veiculo));
    for (int i = 0; i < n; i++) {
        fscanf(arquivo, "%s %d %d", veiculos[i].placa, &veiculos[i].pesoMax, &veiculos[i].volumeMax);
    }
    
    fscanf(arquivo, "%d", &m);
    Pacote *pacotes = malloc(m * sizeof(Pacote));
    for (int i = 0; i < m; i++) {
        fscanf(arquivo, "%s %f %d %d", pacotes[i].codigo, &pacotes[i].valor, &pacotes[i].peso, &pacotes[i].volume);
        pacotes[i].indiceOriginal = i;
        pacotes[i].alocado = 0;
    }
    
    for (int i = 0; i < n; i++) {
        Pacote pacotes_selecionados[m];
        int count_selecionados = 0;
        float valor_total = 0;

        mochila_3d(pacotes, m, veiculos[i].pesoMax, veiculos[i].volumeMax, pacotes_selecionados, &count_selecionados, &valor_total);

        qsort(pacotes_selecionados, count_selecionados, sizeof(Pacote), comparar_por_indice_original);

        char pacotes_alocados[100000] = "";
        for (int k = 0; k < count_selecionados; k++) {
            if (k > 0) strcat(pacotes_alocados, ",");
            strcat(pacotes_alocados, pacotes_selecionados[k].codigo);
        }
        
        fprintf(saida, "[%s]R$%.2f,%dKG,%dL->%s\n", veiculos[i].placa, valor_total, veiculos[i].pesoMax, veiculos[i].volumeMax, pacotes_alocados);
    }
    
    imprimir_pendentes(pacotes, m, saida);
    
    free(veiculos);
    free(pacotes);
    fclose(arquivo);
    fclose(saida);
    return 0;
}