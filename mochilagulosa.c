#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para representar um pacote
typedef struct {
    char codigo[14];
    float valor;
    int peso;
    int volume;
    int indice_original;  // Índice de leitura original
} Pacote;

// Estrutura para representar um veículo
typedef struct {
    char placa[8];
    int peso_max;
    int volume_max;
} Veiculo;

// Função de mesclagem para ordenar pacotes em ordem decrescente por valor
void merge(Pacote *arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Pacote *L = malloc(n1 * sizeof(Pacote));
    Pacote *R = malloc(n2 * sizeof(Pacote));

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    // Mescla os arrays L e R em ordem decrescente
    while (i < n1 && j < n2) {
        if (L[i].valor >= R[j].valor) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copia os elementos restantes de L, se houver
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copia os elementos restantes de R, se houver
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

// Função de ordenação Merge Sort para pacotes
void merge_sort_pacotes(Pacote *arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // Ordena as duas metades
        merge_sort_pacotes(arr, left, mid);
        merge_sort_pacotes(arr, mid + 1, right);

        // Mescla as metades ordenadas
        merge(arr, left, mid, right);
    }
}

// Função de comparação para ordenar por índice original
int comparar_por_indice_original(const void *a, const void *b) {
    Pacote *p1 = (Pacote *)a;
    Pacote *p2 = (Pacote *)b;
    return p1->indice_original - p2->indice_original;
}

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
    int n, m;
    fscanf(arquivo, "%d", &n);

    Veiculo *veiculos = malloc(n * sizeof(Veiculo));
    for (int i = 0; i < n; i++) {
        fscanf(arquivo, "%s %d %d", veiculos[i].placa, &veiculos[i].peso_max, &veiculos[i].volume_max);
    }

    fscanf(arquivo, "%d", &m);

    Pacote *pacotes = malloc(m * sizeof(Pacote));
    for (int i = 0; i < m; i++) {
        fscanf(arquivo, "%s %f %d %d", pacotes[i].codigo, &pacotes[i].valor, &pacotes[i].peso, &pacotes[i].volume);
        pacotes[i].indice_original = i;  // Salva o índice original de leitura
    }

    // Ordena os pacotes por valor total em ordem decrescente
    merge_sort_pacotes(pacotes, 0, m - 1);

    for (int i = 0; i < n; i++) {
        int peso_total = 0, volume_total = 0;
        float valor_total = 0;
        Pacote pacotes_selecionados[m];
        int count_selecionados = 0;

        for (int j = 0; j < m; j++) {
            if (pacotes[j].peso == -1) continue;
            if (peso_total + pacotes[j].peso <= veiculos[i].peso_max &&
                volume_total + pacotes[j].volume <= veiculos[i].volume_max) {
                peso_total += pacotes[j].peso;
                volume_total += pacotes[j].volume;
                valor_total += pacotes[j].valor;
                pacotes_selecionados[count_selecionados++] = pacotes[j];
                pacotes[j].peso = -1;  // Marca o pacote como alocado
            }
        }

        // Ordena os pacotes selecionados por ordem de índice original
        qsort(pacotes_selecionados, count_selecionados, sizeof(Pacote), comparar_por_indice_original);

        // Gera a string de saída para os pacotes alocados
        char pacotes_alocados[1000000] = "";
        for (int k = 0; k < count_selecionados; k++) {
            if (k > 0) strcat(pacotes_alocados, ",");
            strcat(pacotes_alocados, pacotes_selecionados[k].codigo);
        }

        float peso_percent = (peso_total * 100.0) / veiculos[i].peso_max;
        float volume_percent = (volume_total * 100.0) / veiculos[i].volume_max;

        fprintf(saida, "[%s]R$%.2f,%dKG(%.0f%%),%dL(%.0f%%)->%s\n",
                veiculos[i].placa, valor_total, peso_total, peso_percent, volume_total, volume_percent, pacotes_alocados);
    }

    int primeiro_pendente = 1;  // Para controlar vírgula no início
    fprintf(saida, "PENDENTE:");
    for (int i = 0; i < m; i++) {
        if (pacotes[i].peso != -1) {
            if (!primeiro_pendente) fprintf(saida, ",");
            fprintf(saida, "R$%.2f,%dKG,%dL->%s", pacotes[i].valor, pacotes[i].peso, pacotes[i].volume, pacotes[i].codigo);
            primeiro_pendente = 0;
        }
    }

    free(veiculos);
    free(pacotes);
    fclose(arquivo);
    fclose(saida);

    return 0;
}  como saber se o valor selecionado é mais vantajoso ou não em relação á soma de outros 2,3 e etc pacotes que estão mais adianta na lista lida?