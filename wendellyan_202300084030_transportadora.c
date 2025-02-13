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
    int indice_original;
    int alocado; // Indica se o pacote já foi alocado
} Pacote;

// Estrutura para representar um veículo
typedef struct {
    char placa[8];
    int W;
    int V;
} Veiculo;

// Função de comparação para ordenar por índice original
int comparar_por_indice_original(const void *a, const void *b) {
    Pacote *p1 = (Pacote *)a;
    Pacote *p2 = (Pacote *)b;
    return p1->indice_original - p2->indice_original;
}

// Função para encontrar a melhor combinação de pacotes usando programação dinâmica
void otimizacaoVeiculos(Pacote *pacotes, int m, int W, int V, Pacote *selecionados, int *count, float *valorTotal) {
    float ***matriz = (float ***)malloc((m + 1) * sizeof(float **));
    for (int i = 0; i <= m; i++) {
        matriz[i] = (float **)malloc((W + 1) * sizeof(float *));
        for (int w = 0; w <= W; w++) {
            matriz[i][w] = (float *)malloc((V + 1) * sizeof(float));
            memset(matriz[i][w], 0, (V + 1) * sizeof(float));
        }
    }

    for (int i = 1; i <= m; i++) {
        Pacote pacote = pacotes[i - 1];
        for (int w = 0; w <= W; w++) {
            for (int v = 0; v <= V; v++) {
                if (w >= pacote.peso && v >= pacote.volume && !pacote.alocado) {
                    matriz[i][w][v] = fmax(matriz[i - 1][w][v], matriz[i - 1][w - pacote.peso][v - pacote.volume] + pacote.valor);
                } else {
                    matriz[i][w][v] = matriz[i - 1][w][v];
                }
            }
        }
    }

    *valorTotal = matriz[m][W][V];
    *count = 0;
    int w = W, v = V;
    for (int i = m; i > 0 && w > 0 && v > 0; i--) {
        if (matriz[i][w][v] != matriz[i - 1][w][v]) {
            selecionados[(*count)++] = pacotes[i - 1];
            pacotes[i - 1].alocado = 1;
            w -= pacotes[i - 1].peso;
            v -= pacotes[i - 1].volume;
        }
    }

    for (int i = 0; i <= m; i++) {
        for (int w = 0; w <= W; w++) {
            free(matriz[i][w]);
        }
        free(matriz[i]);
    }
    free(matriz);
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
        fscanf(arquivo, "%s %d %d", veiculos[i].placa, &veiculos[i].W, &veiculos[i].V);
    }
    
    fscanf(arquivo, "%d", &m);
    Pacote *pacotes = malloc(m * sizeof(Pacote));
    for (int i = 0; i < m; i++) {
        fscanf(arquivo, "%s %f %d %d", pacotes[i].codigo, &pacotes[i].valor, &pacotes[i].peso, &pacotes[i].volume);
        pacotes[i].indice_original = i;
        pacotes[i].alocado = 0;
    }
    
    for (int i = 0; i < n; i++) {
        Pacote pacotes_selecionados[m];
        int count_selecionados = 0;
        float valor_total = 0;

        otimizacaoVeiculos(pacotes, m, veiculos[i].W, veiculos[i].V, pacotes_selecionados, &count_selecionados, &valor_total);

        qsort(pacotes_selecionados, count_selecionados, sizeof(Pacote), comparar_por_indice_original);

        fprintf(saida, "[%s]R$%.2f,%dKG,%dL->", veiculos[i].placa, valor_total, veiculos[i].W, veiculos[i].V);
        for (int k = 0; k < count_selecionados; k++) {
            if (k > 0) fprintf(saida, ",");
            fprintf(saida, "%s", pacotes_selecionados[k].codigo);
        }
        fprintf(saida, "\n");
    }
    
    imprimir_pendentes(pacotes, m, saida);
    
    free(veiculos);
    free(pacotes);
    fclose(arquivo);
    fclose(saida);
    return 0;
}
