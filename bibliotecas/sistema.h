int criarRaiz(Bloco *disco) {
    int raiz;
    char permissao[10];

    return 1;
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
    int inodeRaiz, teste;
    char usuario[20], senha[20];

    int i;

    limparTela();
    do {
        leitura(usuario, senha);
    } while (!login(senha));
    limparTela();
    inicializarDisco(disco, tamDisco);
    criarListaBlocosLivres(disco, tamDisco);
    exibirPilhas(disco);
    /*for (inodeRaiz = 0; inodeRaiz < 5; inodeRaiz++ ) {
        teste = popBlocoLivre(disco);
        printf("%d\n",teste);
    }
    exibirPilhas(disco);*/
    inodeRaiz = criarRaiz(disco);
    execTerminal(usuario, tamDisco, inodeRaiz);
}