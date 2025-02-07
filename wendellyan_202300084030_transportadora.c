#include <stdio.h>
#include <stdlib.h>


int fibonacci(int n){
    static int *vetor = NULL;
    if (vetor == NULL) {
        vetor = (int*)malloc(n+1 * sizeof(int));
    }    
    if(n<=1){
        return n;
    }
    vetor[0] = 0,vetor[1] = 1;

    for(int i = 2;i<=n;i++){
        vetor[i] = vetor[i-1] + vetor[i-2];
    }
    return vetor[n];
}



int main() {
    int num = 12;
    printf("fibonacci de %d e %d\n", num, fibonacci(num));



    return 0;
}