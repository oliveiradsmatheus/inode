int criarRaiz(Bloco *disco, char *usuario) {
    int raiz;

    raiz = criarInode(disco, usuario, 'd', 1, -1, "");
    return raiz;
}

void execTerminal(Bloco *disco, int endRaiz, char *usuario, int tamDisco) {
    char c, comando[30], caminho[50] = "/"; //, diretorio[14], raiz;
    char nome[30];
    int i, end = endRaiz;
    //raiz = disco[endRaiz].inode.endDireto[0];
    //iniciarBlocos(usuario, blocos);
    do {
        printf("%s@linux: %s $ ", usuario, caminho);
        scanf(" %[^\n]", comando);
        c = eComando(comando);
        if (c != -1)
            executarComando(disco, end, comando, c);
        else
            printf("bash: %s: comando não encontrado\n", comando);
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
    int endRaiz;
    char usuario[20], senha[20];

    limparTela();
    do {
        leitura(usuario, senha);
    } while (!login(senha));
    limparTela();
    inicializarDisco(disco, tamDisco);
    criarListaBlocosLivres(disco, tamDisco);
    //exibirPilhas(disco);
    endRaiz = criarRaiz(disco, usuario);
    adicionarEntradasRaiz(disco, endRaiz, usuario);
    execTerminal(disco, endRaiz, usuario, tamDisco);
}
