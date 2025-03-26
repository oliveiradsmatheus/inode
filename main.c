#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include "bibliotecas/bloco.h"
#include "bibliotecas/comandos.h"
#include "bibliotecas/sistema.h"

int main(void) {
    int tamDisco;
    setlocale(LC_ALL, "Portuguese");
    printf("Informe o tamanho do Disco: ");
    scanf("%d", &tamDisco);
    executar(tamDisco);
    return 0;
}