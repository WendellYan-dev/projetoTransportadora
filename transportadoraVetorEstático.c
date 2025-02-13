#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição dinâmica das constantes máximas
int MAX_M = 0;
int MAX_W = 0;
int MAX_V = 0;

// Estrutura para representar um pacote
typedef struct {
    char codigo[14];
    float valor;
    int peso;
    int volume;
    int indice_original;
    int alocado;
} Pacote;

// Estrutura para representar um veículo
typedef struct {
    char placa[8];
    int W;
    int V;
} Veiculo;

static float ***matriz;

// Funções de comparação para ordenação
int comparar_valor(const void *a, const void *b) {
    return ((Pacote *)b)->valor - ((Pacote *)a)->valor;
}

int comparar_peso(const void *a, const void *b) {
    return ((Pacote *)b)->peso - ((Pacote *)a)->peso;
}

int comparar_volume(const void *a, const void *b) {
    return ((Pacote *)b)->volume - ((Pacote *)a)->volume;
}

// Função para selecionar os pacotes mais relevantes
void selecionar_pacotes(Pacote *pacotes, int m, Pacote *selecionados, int *count, int k) {
    qsort(pacotes, m, sizeof(Pacote), comparar_valor);
    MAX_M = (k > MAX_M) ? k : MAX_M;
    for (int i = 0; i < k && i < m; i++) selecionados[(*count)++] = pacotes[i];

    qsort(pacotes, m, sizeof(Pacote), comparar_peso);
    MAX_W = (pacotes[0].peso > MAX_W) ? pacotes[0].peso : MAX_W;
    for (int i = 0; i < k && i < m; i++) selecionados[(*count)++] = pacotes[i];

    qsort(pacotes, m, sizeof(Pacote), comparar_volume);
    MAX_V = (pacotes[0].volume > MAX_V) ? pacotes[0].volume : MAX_V;
    for (int i = 0; i < k && i < m; i++) selecionados[(*count)++] = pacotes[i];
}

// Função de otimização usando DP
void otimizacaoVeiculos(Pacote *pacotes, int m, int W, int V, Pacote *selecionados, int *count, float *valorTotal) {
    matriz = (float ***)malloc((MAX_M + 1) * sizeof(float **));
    for (int i = 0; i <= MAX_M; i++) {
        matriz[i] = (float **)malloc((MAX_W + 1) * sizeof(float *));
        for (int w = 0; w <= MAX_W; w++) {
            matriz[i][w] = (float *)malloc((MAX_V + 1) * sizeof(float));
        }
    }

    for (int i = 0; i <= m; i++) {
        Pacote pacote = pacotes[i];
        for (int w = 0; w <= W; w++) {
            for (int v = 0; v <= V; v++) {
                if (i == 0 || w == 0 || v == 0) {
                    matriz[i][w][v] = 0;
                } else if (w < pacote.peso || v < pacote.volume || pacote.alocado) {
                    matriz[i][w][v] = matriz[i - 1][w][v];
                } else {
                    float valorSemPacote = matriz[i - 1][w][v];
                    float valorComPacote = matriz[i - 1][w - pacote.peso][v - pacote.volume] + pacote.valor;
                    matriz[i][w][v] = (valorComPacote > valorSemPacote) ? valorComPacote : valorSemPacote;
                }
            }
        }
    }

    *valorTotal = matriz[m][W][V];
    *count = 0;
    int w = W, v = V;
    for (int i = m; i > 0 && w > 0 && v > 0; i--) {
        if (matriz[i][w][v] != matriz[i - 1][w][v]) {
            selecionados[(*count)++] = pacotes[i];
            pacotes[i].alocado = 1;
            w -= pacotes[i].peso;
            v -= pacotes[i].volume;
        }
    }
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
    
    int k = m / 4;
    Pacote pacotes_filtrados[3 * k];
    int count_filtrados = 0;
    selecionar_pacotes(pacotes, m, pacotes_filtrados, &count_filtrados, k);
    
    for (int i = 0; i < n; i++) {
        Pacote pacotes_selecionados[count_filtrados];
        int count_selecionados = 0;
        float valor_total = 0;

        otimizacaoVeiculos(pacotes_filtrados, count_filtrados, veiculos[i].W, veiculos[i].V, pacotes_selecionados, &count_selecionados, &valor_total);

        fprintf(saida, "[%s]R$%.2f\n", veiculos[i].placa, valor_total);
    }
    
    free(veiculos);
    free(pacotes);
    fclose(arquivo);
    fclose(saida);
    return 0;
}
