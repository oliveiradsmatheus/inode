#define CABECA_LISTA_BL 0;

void limparTela() {
#ifdef __linux__
    system("clear");
#else
    system("cls");
#endif
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

void executarComando(char *comando, char c) {
    int i = 0, j = 0;;
    char comandoPrincipal[30] = "";

    while (i < strlen(comando) && comando[i] == ' ')
        i++;
    while (i < strlen(comando) && comando[i] > 96 && comando[i] < 123)
        comandoPrincipal[j++] = comando[i++];

    switch (c) {
        case 0:
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
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            if (!strcmp(comando, "clear"))
                limparTela();
            else
                printf("Opcao nao existente para o comando clear\n");
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

void exibirPilhas(Bloco *disco) {
    int i =0;

    while (i != -1) {
        printf("Pilha [%d] \n", disco[i].listaBlocosLivres.topo);
        printf("End. Pilha [%d] \n",  disco[i].listaBlocosLivres.endProx);
        i = disco[i].listaBlocosLivres.endProx;
    }
}

void criarListaBlocosLivres(Bloco *disco, int tamDisco) {
    int i, j, qtde, num;

    i = CABECA_LISTA_BL;
    qtde = tamDisco/10;
    j = qtde;
    while (i < qtde - 1) {
        for (num = 0; num < 10; num++)
            disco[num].listaBlocosLivres.topo = j++;
        disco[i].listaBlocosLivres.endProx = i + 1;
        i++;
    }
    for (num = 0; num < 10; num++)
        disco[num].listaBlocosLivres.topo = j++;
    disco[i].listaBlocosLivres.endProx = -1;
}

void iniciarBlocos(char *usuario, int *blocos) {
    //criar root, dev, bin, home... respectivos inodes e pilhas de blocos livres.
}

void execTerminal(char *usuario, int tamDisco, int inodeRaiz) {
    char comando[30], diretorio[14] = "~/home", c;

    //iniciarBlocos(usuario, blocos);
    do {
        printf("%s@linux: %s $ ", usuario, diretorio);
        scanf(" %[^\n]", comando);
        c = eComando(comando);
        if (c != -1)
            executarComando(comando, c);
        else
            printf("bash: %s: comando nao encontrado\n", comando);
    } while (strcmp(comando, "exit") && strcmp(comando, "poweroff"));
}

void leitura(char *usuario, char *senha) {
    printf("login: ");
    scanf(" %[^\n]", usuario);
    printf("Password: ");
    scanf("%s", senha);
}

char login(char *senha) {
    return !strcmp(senha, "123");
}

int criarRaiz(Bloco *disco) {
    char permissao[10];

    return 1;
}

void executar(int tamDisco) {
    Bloco disco[tamDisco];
    int inodeRaiz;
    char usuario[20], senha[20];

    limparTela();
    do {
        leitura(usuario, senha);
    } while (!login(senha));
    limparTela();
    inicializarDisco(disco, tamDisco);
    criarListaBlocosLivres(disco, tamDisco);
    exibirPilhas(disco);
    inodeRaiz = criarRaiz(disco);
    execTerminal(usuario, tamDisco, inodeRaiz);
}
