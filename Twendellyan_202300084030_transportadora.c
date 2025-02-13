#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VEICULOS 100
#define MAX_PACOTES 1000

typedef struct {
    char placa[10];
    double capacidade_peso;
    double capacidade_volume;
    double peso_atual;
    double volume_atual;
    double valor_total;
    char pacotes_carregados[MAX_PACOTES][20];
    int qtd_pacotes;
} Veiculo;

typedef struct {
    char codigo[20];
    double valor;
    double peso;
    double volume;
} Pacote;

int comparar_pacotes(const void *a, const void *b) {
    Pacote *p1 = (Pacote *)a;
    Pacote *p2 = (Pacote *)b;
    
    if (p2->valor > p1->valor) return 1;
    if (p2->valor < p1->valor) return -1;

    double densidade1 = p1->valor / (p1->peso + p1->volume);
    double densidade2 = p2->valor / (p2->peso + p2->volume);

    return (densidade2 > densidade1) - (densidade2 < densidade1);
}

void carregar_pacotes(Veiculo *veiculos, int n_veiculos, Pacote *pacotes, int n_pacotes) {
    for (int i = 0; i < n_pacotes; i++) {
        int alocado = 0;
        for (int j = 0; j < n_veiculos; j++) {
            if (veiculos[j].peso_atual + pacotes[i].peso <= veiculos[j].capacidade_peso &&
                veiculos[j].volume_atual + pacotes[i].volume <= veiculos[j].capacidade_volume) {
                
                // Adiciona o pacote ao veículo
                strcpy(veiculos[j].pacotes_carregados[veiculos[j].qtd_pacotes], pacotes[i].codigo);
                veiculos[j].qtd_pacotes++;
                
                // Atualiza o peso, volume e valor total do veículo
                veiculos[j].peso_atual += pacotes[i].peso;
                veiculos[j].volume_atual += pacotes[i].volume;
                veiculos[j].valor_total += pacotes[i].valor;
                
                alocado = 1;
                break;
            }
        }
    }
}

void merge(Pacote arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Pacote *L = (Pacote*) malloc(n1 * sizeof(Pacote));
    Pacote *R = (Pacote*) malloc(n2 * sizeof(Pacote));

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (comparar_pacotes(&L[i], &R[j]) <= 0) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(Pacote arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void imprimir_resultado(Veiculo *veiculos, int n_veiculos, Pacote *pacotes, int n_pacotes) {
    for (int i = 0; i < n_veiculos; i++) {
        printf("[%s]R$%.2f,%.0fKG(%.0f%%),%.0fL(%.0f%%)->", veiculos[i].placa, veiculos[i].valor_total,
               veiculos[i].peso_atual, (veiculos[i].peso_atual / veiculos[i].capacidade_peso) * 100,
               veiculos[i].volume_atual, (veiculos[i].volume_atual / veiculos[i].capacidade_volume) * 100);
        
        for (int j = 0; j < veiculos[i].qtd_pacotes; j++) {
            printf("%s%s", veiculos[i].pacotes_carregados[j], (j < veiculos[i].qtd_pacotes - 1) ? "," : "");
        }
        printf("\n"); // Adiciona uma nova linha após a impressão dos pacotes do veículo
    }
    
    // Imprime pacotes pendentes
    printf("PENDENTE:");
    for (int i = 0; i < n_pacotes; i++) {
        int alocado = 0;
        for (int j = 0; j < n_veiculos; j++) {
            for (int k = 0; k < veiculos[j].qtd_pacotes; k++) {
                if (strcmp(pacotes[i].codigo, veiculos[j].pacotes_carregados[k]) == 0) {
                    alocado = 1;
                    break;
                }
            }
            if (alocado) break;
        }
        

        if (!alocado) {
            printf("R$%.2f,%.2fKG,%.2fL->%s\n", pacotes[i].valor, pacotes[i].peso, pacotes[i].volume, pacotes[i].codigo);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("ERRO: Caminho do arquivo não fornecido.\n");
        return 0;
    }

    FILE* arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        printf("ERRO: Não foi possível abrir o arquivo.\n");
        return 0;
    }

    int n_veiculos, n_pacotes;
    Veiculo veiculos[MAX_VEICULOS];
    Pacote pacotes[MAX_PACOTES];

    fscanf(arquivo, "%d", &n_veiculos);
    for (int i = 0; i < n_veiculos; i++) {
        fscanf(arquivo, "%s %lf %lf", veiculos[i].placa, &veiculos[i].capacidade_peso, &veiculos[i].capacidade_volume);
        veiculos[i].peso_atual = 0;
        veiculos[i].volume_atual = 0;
        veiculos[i].valor_total = 0;
        veiculos[i].qtd_pacotes = 0;
    }

    fscanf(arquivo, "%d", &n_pacotes);
    for (int i = 0; i < n_pacotes; i++) {
        fscanf(arquivo, "%s %lf %lf %lf", pacotes[i].codigo, &pacotes[i].valor, &pacotes[i].peso, &pacotes[i].volume);
    }

    // Ordena os pacotes com base no valor e densidade
    mergeSort(pacotes, 0, n_pacotes - 1);
    
    // Carrega os pacotes nos veículos
    carregar_pacotes(veiculos, n_veiculos, pacotes, n_pacotes);
    
    // Imprime o resultado
    imprimir_resultado(veiculos, n_veiculos, pacotes, n_pacotes);

    // Fecha o arquivo
    fclose(arquivo);
    
    return 0;
}