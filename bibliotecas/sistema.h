int criarRaiz(Bloco *disco, char *usuario) {
    int raiz;

    raiz = criarInode(disco, usuario, 'd', 1, -1, "");
    return raiz;
}

char caminhoUsuario(char *caminho, char *usuario, char *caminhoAbreviado) {
    char caminhoUsuario[100] = "/home/";
    int i = 0, j = 2;

    strcat(caminhoUsuario, usuario);
    while (i < strlen(caminho) && caminho[i] == caminhoUsuario[i])
        i++;

    if (i == strlen(caminhoUsuario)) {
        strcpy(caminhoAbreviado, "~/");
        i++;
        while (i < strlen(caminho))
            caminhoAbreviado[j++] = caminho[i++];
        caminhoAbreviado[j] = '\0';
        return 1;
    }
    return 0;
}

void execTerminal(Bloco *disco, int endRaiz, int endUsuario, char *usuario, int tamDisco) {
    char c, comando[100], caminho[200] = "/home/", caminhoAbreviado[190] = "~/";
    int endAtual = endUsuario;

    strcat(caminho, usuario);
    do {
        if (caminhoUsuario(caminho, usuario, caminhoAbreviado)) {
            printf("%s%s@linux%s: %s%s%s $ ", AMARELO, usuario, RESET, AZUL, caminhoAbreviado, RESET);
        } else
            printf("%s%s@linux%s: %s%s%s $ ", AMARELO, usuario, RESET, AZUL,caminho, RESET);
        scanf(" %[^\n]", comando);
        c = eComando(comando);
        if (c != -1) {
            endAtual = executarComando(disco, usuario, endRaiz, endUsuario, endAtual, comando, c, tamDisco, caminho);
        } else
            printf("bash: %s: comando nao encontrado\n", comando);
    } while (strcmp(comando, "exit") && strcmp(comando, "poweroff"));
}

char login(char *senha) {
    return !strcmp(senha, "123");
}

void leitura(char *usuario, char *senha) {
    printf("login: ");
    scanf(" %[^\n]", usuario);
    printf("Password: ");
    scanf("%s", senha);
}

void executar(int tamDisco) {
    Bloco disco[tamDisco];
    int endRaiz, endUsuario;
    char usuario[20], senha[20];

    limparTela();
    do {
        leitura(usuario, senha);
    } while (!login(senha));
    limparTela();
    inicializarDisco(disco, tamDisco);
    criarListaBlocosLivres(disco, tamDisco);
    endRaiz = criarRaiz(disco, usuario);
    endUsuario = adicionarEntradasRaiz(disco, endRaiz, usuario);
    execTerminal(disco, endRaiz, endUsuario, usuario, tamDisco);
}
