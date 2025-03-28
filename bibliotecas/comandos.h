void limparTela() {
#ifdef __linux__
    system("clear");
#else
    system("cls");
#endif
}

void listar(Bloco *disco, int end) {
    int i, j;

    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != -1) {
        for (j = 2; j < disco[disco[end].inode.endDireto[i]].dir.TL; j++)
            if (!dirVazio(disco[disco[end].inode.endDireto[i]]))
                printf("%s     ", disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome);
        printf("\n");
        i++;
    }

    /*endAtual = disco[end].inode.endDireto[i];
    while (endAtual != endNulo() && i < 5) {
        for (j = 2; j < disco[endAtual].dir.TL; j++)
            printf("%s     ", disco[endAtual].dir.arquivo[j].nome);
        printf("\n");
        i++;
        endAtual = disco[end].inode.endDireto[i];*/
}

char validarCriacao(char *comando, char *nomeArq, int *tam) {
    int i, j;
    char tamStr[100], flag = 1;

    i = 6;
    j = 0;
    while (i < strlen(comando) && comando[i] != ' ')
        nomeArq[j++] = comando[i++];
    nomeArq[j] = '\0';
    i++;
    if (i < strlen(comando)) {
        j = 0;
        while (i < strlen(comando))
            tamStr[j++] = comando[i++];
        tamStr[j] = '\0';
        if (i == strlen(comando)) {
            for (i = 0; i < strlen(tamStr) && flag; i++)
                if (tamStr[i] < '0' || tamStr[i] > '9')
                    flag = 0;
                else
                    *tam = atoi(tamStr);
            return flag;
        }
    }
    return 0;
}

void touch(Bloco *disco, int end, char *usuario, char *nomeArq, int tam) {
    if ((float)tam / (float) 10 < qtdeBlocosLivres(disco))
        adicionarEntrada(disco, end, usuario, nomeArq, '-', tam);
    else
        printf("Espaço em disco insuficiente!\n");
}

char eComando(char *comando) {
    int i = 0, j = 0;
    char comandoPrincipal[30] = "";

    while (i < strlen(comando) && comando[i] == ' ')
        i++;
    while (i < strlen(comando) && comando[i] > 96 && comando[i] < 123)
        comandoPrincipal[j++] = comando[i++];
    if (!strcmp(comandoPrincipal, "ls"))
        return 0;
    else if (!strcmp(comandoPrincipal, "mkdir"))
        return 1;
    else if (!strcmp(comandoPrincipal, "rmdir"))
        return 2;
    else if (
        !strcmp(comandoPrincipal, "rm"))
        return 3;
    else if (
        !strcmp(comandoPrincipal, "cd"))
        return 4;
    else if (
        !strcmp(comandoPrincipal, "link"))
        return 5;
    else if (
        !strcmp(comandoPrincipal, "bad"))
        return 6;
    else if (
        !strcmp(comandoPrincipal, "unlink"))
        return 7;
    else if (
        !strcmp(comandoPrincipal, "touch"))
        return 8;
    else if (
        !strcmp(comandoPrincipal, "df"))
        return 9;
    else if (
        !strcmp(comandoPrincipal, "clear"))
        return 10;
    else if (!strcmp(comandoPrincipal, "chmod"))
        return 11;
    else if (!strcmp(comandoPrincipal, "vi"))
        return 12;
    else if (!strcmp(comandoPrincipal, "pwd"))
        return 13;
    else if (!strcmp(comandoPrincipal, "tree"))
        return 14;
    else if (!strcmp(comandoPrincipal, "exit") || !strcmp(comandoPrincipal, "poweroff"))
        return 15;
    return -1;
}

void executarComando(Bloco *disco, char *usuario, int end, char *comando, char c, int tamDisco) {
    int i = 0, j = 0, tam;
    char comandoPrincipal[30] = "", nomeArq[TAM_MAX_NOME];

    while (i < strlen(comando) && comando[i] == ' ')
        i++;
    while (i < strlen(comando) && comando[i] > 96 && comando[i] < 123)
        comandoPrincipal[j++] = comando[i++];

    switch (c) {
        case 0:
            if (!strcmp(comando, "ls"))
                listar(disco, end);
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 6:
            setBad(disco, comando, tamDisco);
            break;
        case 7:
            break;
        case 8:
            if (validarCriacao(comando, nomeArq, &tam))
                touch(disco, end, usuario, nomeArq, tam);
            else
                printf("Opção não existente para o comando touch\n");
            break;
        case 9:
            break;
        case 10:
            if (!strcmp(comando, "clear"))
                limparTela();
            else
                printf("Opção não existente para o comando clear\n");
            break;
        case 11:
            break;
        case 12:
            break;
        case 13:
            break;
        case 14:
            break;
    }
}
