#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef __linux__
#else
#include <windows.h>
#endif
#include "bibliotecas/cores.h"
#include "bibliotecas/bloco.h"
#include "bibliotecas/comandos.h"
#include "bibliotecas/sistema.h"

int main(void) {
    habilitarCores();
    int tamDisco;

    printf("Informe o tamanho do Disco: ");
    scanf("%d", &tamDisco);
    executar(tamDisco);

    return 0;
}
