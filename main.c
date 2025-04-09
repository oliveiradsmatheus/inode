#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bibliotecas/bloco.h"
#include "bibliotecas/comandos.h"
#include "bibliotecas/sistema.h"
#include "bibliotecas/cores.h"

int main(void) {
    habilitarCores();
    int tamDisco;
    printf(VERMELHO"Informe o tamanho do Disco: "RESET);
    scanf("%d", &tamDisco);
    executar(tamDisco);
    return 0;
}