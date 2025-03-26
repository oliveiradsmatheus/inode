#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "bibliotecas/bloco.h"
#include "bibliotecas/comandos.h"
#include "bibliotecas/sistema.h"

int main(void) {
    int tamDisco;
    printf("Informe o tamanho do Disco: ");
    scanf("%d", &tamDisco);
    executar(tamDisco);
    return 0;
}