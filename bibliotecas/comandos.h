void limparTela() {
#ifdef __linux__
    system("clear");
#else
    system("cls");
#endif
}

void mostrarBlocos(Bloco *disco, int tamDisco) {
    int i;
    for (i = 0; i < tamDisco; i++) {
        if (disco[i].bad)
            printf("[%d: B]  ", i);
        else if (blocoLivre(disco, i)) {
            printf("[%d: F]  ", i);
        } else
            printf("[%d: A]  ", i);
        if (i != 0 && !(i % 9))
            printf("\n");
    }
    printf("\n");
}

void livresOcupadosBytes(Bloco *disco, int tamDisco) {
    int ocupado = 0, i;

    for (i = 0; i < tamDisco; i++) {
        if (!blocoLivre(disco, i)) {
            if (!arquivo(disco, i))
                if (disco[i].inode.permissao[0] == '\0' || disco[i].inode.permissao[0] == 'd' || disco[i].inode.
                    permissao[0] == 'l')
                    ocupado += 10;
                else
                    ocupado += 10 + disco[i].inode.tamanho;
        }
    }
    printf("Espaço livre em disco: %d bytes\tEspaço ocupado: %d bytes\n", tamDisco * 10 - ocupado, ocupado);
}

void listar(Bloco *disco, int end) {
    int i, j, k, l, endSimples, endDuplo, endTriplo;
    char vazio = 1;

    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != endNulo()) {
        for (j = 2; j < disco[disco[end].inode.endDireto[i]].dir.TL; j++)
            if (!dirVazio(disco[disco[end].inode.endDireto[i]])) {
                printf("%s     ", disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome);
                vazio = 0;
            }
        if (!vazio)
            printf("\n");
        i++;
    }
    i = 0;
    if (disco[end].inode.endSimplesIndireto != endNulo()) {
        endSimples = disco[end].inode.endSimplesIndireto;
        while (i < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[i] !=
               endNulo()) {
            for (j = 2; j < disco[disco[endSimples].inodeIndireto.endInd[i]].dir.TL; j++)
                if (!dirVazio(disco[disco[endSimples].inodeIndireto.endInd[i]])
                    && !bad(
                        disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].
                            endInode]))
                    printf("%s     ",
                           disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].
                           nome);
            printf("\n");
            i++;
        }
        if (disco[end].inode.endDuploIndireto != endNulo()) {
            i = 0;
            endDuplo = disco[end].inode.endDuploIndireto;
            while (i < QTDE_INODE_INDIRETO && disco[endDuplo].inodeIndireto.endInd[i] != endNulo()) {
                j = 0;
                endSimples = disco[endDuplo].inodeIndireto.endInd[i];
                while (j < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[j] != endNulo()) {
                    for (k = 2; k < disco[disco[endSimples].inodeIndireto.endInd[j]].dir.TL; k++)
                        if (!dirVazio(disco[disco[endSimples].inodeIndireto.endInd[j]]))
                            printf("%s     ", disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].nome);
                    printf("\n");
                    j++;
                }
                i++;
            }
            if (disco[end].inode.endTriploIndireto != endNulo()) {
                i = 0;
                endTriplo = disco[end].inode.endTriploIndireto;
                while (i < QTDE_INODE_INDIRETO && disco[endTriplo].inodeIndireto.endInd[i] != endNulo()) {
                    j = 0;
                    endDuplo = disco[endTriplo].inodeIndireto.endInd[i];
                    while (j < QTDE_INODE_INDIRETO && disco[endDuplo].inodeIndireto.endInd[j] != endNulo()) {
                        k = 0;
                        endSimples = disco[endDuplo].inodeIndireto.endInd[j];
                        while (k < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[k] != endNulo()) {
                            for (l = 2; l < disco[disco[endSimples].inodeIndireto.endInd[k]].dir.TL; l++)
                                if (!dirVazio(disco[endSimples]))
                                    printf("%s     ",
                                           disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].nome);
                            printf("\n");
                            k++;
                        }
                        j++;
                    }
                    i++;
                }
            }
        }
    }
}

void rmdir(Bloco *disco, int endEntrada, char *nomeDir) {
    int end, pos, i = 0, inodeDir;

    if (buscaArquivo(disco, endEntrada, nomeDir, &pos, &end) == -1)
        printf("rmdir: falhou em remover '%s': Arquivo ou diretório inexistente\n", nomeDir);
    else {
        inodeDir = disco[end].dir.arquivo[pos].endInode;
        if (dirVazio(disco[disco[inodeDir].inode.endDireto[0]])) {
            i = pos;
            while (i < disco[end].dir.TL - 1) {
                strcpy(disco[end].dir.arquivo[i].nome, disco[end].dir.arquivo[i + 1].nome);
                disco[end].dir.arquivo[i].endInode = disco[end].dir.arquivo[i + 1].endInode;
                i++;
            }
            disco[end].dir.TL--;
            pushBlocoLivre(disco, disco[inodeDir].inode.endDireto[0]);
            pushBlocoLivre(disco, inodeDir);
        } else
            printf("rmdir: falhou em remover '%s': Diretório não vazio\n", nomeDir);
    }
}

void listarAtributos(Bloco *disco, int end) {
    int i, j, k, l, endInode, endSimples, endDuplo, endTriplo;

    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != endNulo()) {
        for (j = 2; j < disco[disco[end].inode.endDireto[i]].dir.TL; j++)
            if (!dirVazio(disco[disco[end].inode.endDireto[i]])) {
                endInode = disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode;
                printf("%s\t%d\t%s\t%s\tCriado: %s\t Último acesso: %s\tÚltima alteração: %s\t%s\n",
                       disco[endInode].inode.permissao, disco[endInode].inode.contadorLink,
                       disco[endInode].inode.proprietario, disco[endInode].inode.grupo, disco[endInode].inode.criacao,
                       disco[endInode].inode.ultimoAcesso, disco[endInode].inode.ultimaAlteracao,
                       disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome);
            }
        i++;
    }
    i = 0;
    if (disco[end].inode.endSimplesIndireto != endNulo()) {
        endSimples = disco[end].inode.endSimplesIndireto;
        while (i < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[i] != endNulo()) {
            for (j = 2; j < disco[disco[endSimples].inodeIndireto.endInd[i]].dir.TL; j++)
                if (!dirVazio(disco[disco[endSimples].inodeIndireto.endInd[i]])) {
                    endInode = disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode;
                    printf("%s\t%d\t%s\t%s\tCriado: %s\t Último acesso: %s\tÚltima alteração: %s\t%s\n",
                           disco[endInode].inode.permissao, disco[endInode].inode.contadorLink,
                           disco[endInode].inode.proprietario, disco[endInode].inode.grupo,
                           disco[endInode].inode.criacao,
                           disco[endInode].inode.ultimoAcesso, disco[endInode].inode.ultimaAlteracao,
                           disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].
                           nome);
                }
            i++;
        }
        if (disco[end].inode.endDuploIndireto != endNulo()) {
            i = 0;
            endDuplo = disco[end].inode.endDuploIndireto;
            while (i < QTDE_INODE_INDIRETO && disco[endDuplo].inodeIndireto.endInd[i] != endNulo()) {
                j = 0;
                endSimples = disco[endDuplo].inodeIndireto.endInd[i];
                while (j < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[j] != endNulo()) {
                    for (k = 2; k < disco[disco[endSimples].inodeIndireto.endInd[j]].dir.TL; k++)
                        if (!dirVazio(disco[disco[endSimples].inodeIndireto.endInd[j]])) {
                            endInode = disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].endInode;
                            printf("%s\t%d\t%s\t%s\tCriado: %s\t Último acesso: %s\tÚltima alteração: %s\t%s\n",
                                   disco[endInode].inode.permissao, disco[endInode].inode.contadorLink,
                                   disco[endInode].inode.proprietario, disco[endInode].inode.grupo,
                                   disco[endInode].inode.criacao,
                                   disco[endInode].inode.ultimoAcesso, disco[endInode].inode.ultimaAlteracao,
                                   disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].nome);
                        }
                    j++;
                }
                i++;
            }
            if (disco[end].inode.endTriploIndireto != endNulo()) {
                i = 0;
                endTriplo = disco[end].inode.endTriploIndireto;
                while (i < QTDE_INODE_INDIRETO && disco[endTriplo].inodeIndireto.endInd[i] != endNulo()) {
                    j = 0;
                    endDuplo = disco[endTriplo].inodeIndireto.endInd[i];
                    while (j < QTDE_INODE_INDIRETO && disco[endDuplo].inodeIndireto.endInd[j] != endNulo()) {
                        k = 0;
                        endSimples = disco[endDuplo].inodeIndireto.endInd[j];
                        while (k < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[k] != endNulo()) {
                            for (l = 2; l < disco[disco[endSimples].inodeIndireto.endInd[k]].dir.TL; l++)
                                if (!dirVazio(disco[endSimples])) {
                                    endInode = disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].endInode;
                                    printf("%s\t%d\t%s\t%s\tCriado: %s\t Último acesso: %s\tÚltima alteração: %s\t%s\n",
                                           disco[endInode].inode.permissao, disco[endInode].inode.contadorLink,
                                           disco[endInode].inode.proprietario, disco[endInode].inode.grupo,
                                           disco[endInode].inode.criacao,
                                           disco[endInode].inode.ultimoAcesso, disco[endInode].inode.ultimaAlteracao,
                                           disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].nome);
                                }
                            k++;
                        }
                        j++;
                    }
                    i++;
                }
            }
        }
    }
}

char ultimoArquivo(Bloco *disco, int endDir, int posDir) {
    int i = posDir + 1;
    while (i < disco[endDir].dir.TL && disco[disco[endDir].dir.arquivo[i].endInode].inode.permissao[0] == '-')
        i++;
    if (i == disco[endDir].dir.TL)
        return 1;
    return 0;
}

void arvore(Bloco *disco, int end, int nivel, int *vet) {
    int i = 1, j, k;
    char espaco[400] = "";

    if (disco[disco[end].inode.endDireto[0]].dir.TL == 3)
        vet[nivel] = 1; // 1 significa o último diretório!

    while (i < nivel) {
        if (vet[i] == 0)
            strcat(espaco, "│\t");
        else
            strcat(espaco, "   \t");
        i++;
    }
    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != endNulo()) {
        for (j = 2; j < disco[disco[end].inode.endDireto[i]].dir.TL; j++) {
            if (disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.permissao[0] == 'd') {
                k = nivel * 4 - 4;
                espaco[k] = '\0';
                if (ultimoArquivo(disco, disco[end].inode.endDireto[i], j)) {
                    vet[nivel] = 1; // 1 significa o último diretório!
                    strcat(espaco, "└── ");
                } else
                    strcat(espaco, "├── ");
                printf("%s%s\n", espaco, disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome);
                arvore(disco, disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode, nivel + 1, vet);
            }
        }
        i++;
    }
}

char linkValido(char *comando, char *origem, char *destino) {
    char tipo = 0;
    int i = 8, j;

    printf("%s\n", comando);

    if (comando[5] == '-' && comando[6] == 's')
        tipo = 1;
    else if (comando[5] == '-' && comando[6] == 'h')
        tipo = 2;

    j = 0;
    while (i < strlen(comando) && comando[i] != ' ')
        origem[j++] = comando[i++];
    origem[j] = '\0';
    i++;
    j = 0;
    while (i < strlen(comando) && comando[i] != ' ')
        destino[j++] = comando[i++];
    destino[j] = '\0';

    if (i == strlen(comando))
        return tipo;
    return -1;
}

char validarRemocao(char *comando, char *nomeArquivo) {
    int i = 6, j = 0;

    while (i < strlen(comando) && comando[i] != ' ')
        nomeArquivo[j++] = comando[i++];
    nomeArquivo[j] = '\0';
    if (i == strlen(comando))
        return 1;
    return 0;
}

char validarAlteracaoPermissao(char *comando, char *permissao) {
    int i = 7, j = 0;

    while (i < strlen(comando))
        permissao[j++] = comando[i++];
    if (permissaoValida(permissao))
        return 1;
    return 0;
}

char validarCriacaoArq(char *comando, char *nomeArq, int *tam) {
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

void criarLinkFisico() {
}

void criarLinkSimbolico(Bloco *disco, int endDir, char *origem, char *destino) {
}

char validarCriacaoDir(char *comando, char *nomeDir) {
    int i, j;
    char flag = 0;

    i = 6;
    j = 0;
    while (i < strlen(comando) && comando[i] != ' ')
        nomeDir[j++] = comando[i++];
    nomeDir[j] = '\0';
    if (i == strlen(comando))
        flag = 1;
    return flag;
}

void visualizarArquivo(Bloco *disco, int endDir, char *comando) {
    char busca, nomeArquivo[15];
    int i = 3, j = 0, end, pos;

    while (i < strlen(comando) && comando[i] != ' ')
        nomeArquivo[j++] = comando[i++];
    nomeArquivo[j] = '\0';

    if (i == strlen(comando) && strlen(nomeArquivo) < 15) {
        busca = buscaArquivo(disco, endDir, nomeArquivo, &pos, &end);
        if (busca != -1) {
            if (disco[disco[end].dir.arquivo[pos].endInode].inode.permissao[0] == '-')
                printf("Arquivo %s visualizado\n", disco[end].dir.arquivo[pos].nome);
            else
                printf("O arquivo %s é um diretório\n", disco[end].dir.arquivo[pos].nome);
        } else
            printf("Arquivo não encontrado\n");
    } else
        printf("Arquivo não encontrado\n");
}

void touch(Bloco *disco, int end, char *usuario, char *nomeArq, int tam) {
    int pos, endArq;

    if (buscaArquivo(disco, end, nomeArq, &pos, &endArq) == -1) {
        if ((float) tam / (float) 10 < qtdeBlocosLivres(disco))
            adicionarEntrada(disco, end, usuario, nomeArq, 'a', tam);
        else
            printf("Espaço em disco insuficiente!\n");
    } else {
        if ((float) tam / (float) 10 < qtdeBlocosLivres(disco))
            disco[disco[endArq].dir.arquivo[pos].endInode].inode.tamanho = tam;
        else
            printf("Espaço em disco insuficiente!\n");
    }
}

void mkdir(Bloco *disco, int end, char *usuario, char *nomeDir) {
    int pos, endArq;

    if (buscaArquivo(disco, end, nomeDir, &pos, &endArq) == -1) {
        if ((float) 1 / (float) 10 < qtdeBlocosLivres(disco))
            adicionarEntrada(disco, end, usuario, nomeDir, 'd', 1);
        else
            printf("Espaço em disco insuficiente!\n");
    } else
        printf("mkdir: não foi possível criar o diretório “%s”: Arquivo existe\n", nomeDir);
}

void proxEntrada(char *caminho, char *entrada) {
    int i = 0, j = 0;

    if (caminho[0] == '/') {
        i = 0;
        while (i < strlen(caminho) - 1)
            caminho[i++] = caminho[i + 1];
        caminho[i] = '\0';
    }

    i = 0;
    while (i < strlen(caminho) && caminho[i] != '/')
        entrada[j++] = caminho[i++];
    entrada[j] = '\0';

    i = 0;
    while (i < strlen(caminho) - strlen(entrada))
        caminho[i++] = caminho[i + strlen(entrada)];
    caminho[i] = '\0';
}

int navegar(Bloco *disco, int raiz, int endUsuario, int endAtual, char *comando, char *usuario, char *caminhoTotal) {
    char caminho[50], entrada[15] = "", caminhoUsuario[20] = "/home/", caminhoTotalAux[50];
    int i = 3, j, busca, pos, novoEnd = endAtual, endBusca;

    j = 0;
    while (i < strlen(comando))
        caminho[j++] = comando[i++];
    caminho[j] = '\0';
    strcat(caminhoUsuario, usuario);

    if (strlen(caminho) > 0) {
        if (!strcmp(caminho, "/")) {
            novoEnd = raiz;
            strcpy(caminhoTotal, "/");
        } else {
            if (caminho[0] == '/') {
                // Navegar pelo caminho absoluto
                strcpy(caminhoTotalAux, "/");
                proxEntrada(caminho, entrada);
                endBusca = raiz;
                busca = buscaArquivo(disco, endBusca, entrada, &pos, &endAtual);
                while (busca != -1 && strlen(entrada) > 0) {
                    if (!strcmp(entrada, "..")) {
                        i = strlen(caminhoTotalAux);
                        while (i > 1 && caminhoTotalAux[i] != '/') {
                            caminhoTotalAux[i] = '\0';
                            i--;
                        }
                        caminhoTotalAux[i] = '\0';
                    } else {
                        if (strcmp(entrada, "."))
                            if (!strcmp(caminhoTotalAux, "/"))
                                strcat(caminhoTotalAux, entrada);
                            else {
                                strcat(caminhoTotalAux, "/");
                                strcat(caminhoTotalAux, entrada);
                            }
                    }
                    endBusca = disco[endAtual].dir.arquivo[pos].endInode;
                    proxEntrada(caminho, entrada);
                    if (strlen(entrada) > 0)
                        busca = buscaArquivo(disco, endBusca, entrada, &pos, &endAtual);
                }
                if (busca == -1)
                    printf("bash: cd: Arquivo ou diretório inexistente\n");
                else {
                    novoEnd = disco[endAtual].dir.arquivo[pos].endInode;
                    strcpy(caminhoTotal, caminhoTotalAux);
                }
            } else {
                strcpy(caminhoTotalAux, caminhoTotal);
                if (!strcmp(caminho, ".")) {
                    novoEnd = disco[disco[endAtual].inode.endDireto[0]].dir.arquivo[0].endInode;
                } else {
                    if (!strcmp(caminho, "..")) {
                        novoEnd = disco[disco[endAtual].inode.endDireto[0]].dir.arquivo[1].endInode;
                        i = strlen(caminhoTotal);
                        while (i > 1 && caminhoTotal[i] != '/') {
                            caminhoTotal[i] = '\0';
                            i--;
                        }
                        caminhoTotal[i] = '\0';
                    } else {
                        proxEntrada(caminho, entrada);
                        endBusca = endAtual;
                        busca = buscaArquivo(disco, endBusca, entrada, &pos, &endAtual);
                        while (busca != -1 && strlen(entrada) > 0) {
                            if (!strcmp(entrada, "..")) {
                                i = strlen(caminhoTotalAux);
                                while (i > 1 && caminhoTotalAux[i] != '/') {
                                    caminhoTotalAux[i] = '\0';
                                    i--;
                                }
                                caminhoTotalAux[i] = '\0';
                            } else {
                                if (strcmp(entrada, "."))
                                    if (!strcmp(caminhoTotalAux, "/"))
                                        strcat(caminhoTotalAux, entrada);
                                    else {
                                        strcat(caminhoTotalAux, "/");
                                        strcat(caminhoTotalAux, entrada);
                                    }
                            }
                            endBusca = disco[endAtual].dir.arquivo[pos].endInode;
                            proxEntrada(caminho, entrada);
                            if (strlen(entrada) > 0)
                                busca = buscaArquivo(disco, endBusca, entrada, &pos, &endAtual);
                        }
                        if (busca == -1)
                            printf("bash: cd: Arquivo ou diretório inexistente\n");
                        else {
                            novoEnd = disco[endAtual].dir.arquivo[pos].endInode;
                            strcpy(caminhoTotal, caminhoTotalAux);
                        }
                    }
                }
            }
        }
    } else if (!strcmp(comando, "cd")) {
        novoEnd = endUsuario;
        strcpy(caminhoTotal, caminhoUsuario);
    }
    if (bad(disco[novoEnd])) {
        novoEnd = endUsuario;
        strcpy(caminhoTotal, caminhoUsuario);
        printf("Erro: Diretório corrompido\n");
        return novoEnd;
    }
    if (disco[novoEnd].inode.permissao[0] == 'd')
        return novoEnd;
    return -1;
}

char eComando(char *comando) {
    int i = 0, j = 0, c = -1;
    char comandoPrincipal[30] = "";

    while (i < strlen(comando) && comando[i] == ' ')
        i++;
    while (i < strlen(comando) && comando[i] > 96 && comando[i] < 123)
        comandoPrincipal[j++] = comando[i++];
    if (!strcmp(comandoPrincipal, "ls"))
        c = 0;
    else if (!strcmp(comandoPrincipal, "mkdir"))
        c = 1;
    else if (!strcmp(comandoPrincipal, "rmdir"))
        c = 2;
    else if (
        !strcmp(comandoPrincipal, "rm"))
        c = 3;
    else if (
        !strcmp(comandoPrincipal, "cd"))
        c = 4;
    else if (
        !strcmp(comandoPrincipal, "link"))
        c = 5;
    else if (
        !strcmp(comandoPrincipal, "bad"))
        c = 6;
    else if (
        !strcmp(comandoPrincipal, "unlink"))
        c = 7;
    else if (
        !strcmp(comandoPrincipal, "touch"))
        c = 8;
    else if (
        !strcmp(comandoPrincipal, "df"))
        c = 9;
    else if (
        !strcmp(comandoPrincipal, "clear"))
        c = 10;
    else if (!strcmp(comandoPrincipal, "chmod"))
        c = 11;
    else if (!strcmp(comandoPrincipal, "vi"))
        c = 12;
    else if (!strcmp(comandoPrincipal, "pwd"))
        c = 13;
    else if (!strcmp(comandoPrincipal, "tree"))
        c = 14;
    else if (!strcmp(comandoPrincipal, "blocks"))
        c = 15;
    else if (!strcmp(comandoPrincipal, "stack"))
        c = 16;
    else if (!strcmp(comandoPrincipal, "exit") || !strcmp(comandoPrincipal, "poweroff"))
        c = 17;
    return c;
}

int executarComando(Bloco *disco, char *usuario, int raiz, int endUsuario, int end, char *comando, char c, int tamDisco,
                    char *caminho) {
    int i = 0, tam, endAtual = end, vet[30];
    char nomeArq[TAM_MAX_NOME], permissao[7], origem[50], destino[50], tipoLink;

    switch (c) {
        case 0:
            if (!strcmp(comando, "ls"))
                listar(disco, end);
            else if (!strcmp(comando, "ls -l"))
                listarAtributos(disco, end);
            break;
        case 1:
            if (validarCriacaoDir(comando, nomeArq))
                if (strlen(nomeArq) <= 14)
                    mkdir(disco, end, usuario, nomeArq);
                else
                    printf("Nome de arquivo muito longo\n");
            else
                printf("Opção não existente para o comando mkdir\n");
            break;
        case 2:
            if (validarRemocao(comando, nomeArq))
                rmdir(disco, endAtual, nomeArq);
            break;
        case 3:
            //rm
            break;
        case 4:
            endAtual = navegar(disco, raiz, endUsuario, endAtual, comando, usuario, caminho);
            if (endAtual == -1) {
                printf("bash: cd: %s: Não é um diretório\n", caminho);
                endAtual = end;
                i = strlen(caminho);
                while (caminho[i] != '/')
                    caminho[i--] = '\0';
                caminho[i] = '\0';
            }
            break;
        case 5:
            tipoLink = linkValido(comando, origem, destino);
            if (tipoLink == -1)
                printf("Não foi possível criar o link '%s' para '%s': Arquivo ou diretório inexistente\n", origem,
                       destino);
            else {
                if (tipoLink == 1) {
                    criarLinkSimbolico(disco, end, origem, destino);
                } else {
                    criarLinkFisico(disco, end, destino);
                }
            }
            break;
        case 6:
            setBad(disco, comando, tamDisco);
            break;
        case 7:
            // unlink
            break;
        case 8:
            if (validarCriacaoArq(comando, nomeArq, &tam))
                if (strlen(nomeArq) <= 14)
                    touch(disco, end, usuario, nomeArq, tam);
                else
                    printf("Nome de arquivo muito longo\n");
            else
                printf("Opção não existente para o comando touch\n");
            break;
        case 9:
            if (!strcmp(comando, "df"))
                livresOcupadosBytes(disco, tamDisco);
            break;
        case 10:
            if (!strcmp(comando, "clear"))
                limparTela();
            else
                printf("Opção não existente para o comando clear\n");
            break;
        case 11:
            if (validarAlteracaoPermissao(comando, permissao))
                //chmod(disco, end, nomeArq, permissao);
                break;
        case 12:
            visualizarArquivo(disco, end, comando);
            break;
        case 13:
            if (!strcmp(comando, "pwd"))
                printf("%s\n", caminho);
            else
                printf("Opção não existente para o comando pwd\n");
            break;
        case 14:
            if (!strcmp(comando, "tree")) {
                for (i = 0; i < 30; i++)
                    vet[i] = 0;
                arvore(disco, raiz, 1, vet);
            }
            break;
        case 15:
            if (!strcmp(comando, "blocks"))
                mostrarBlocos(disco, tamDisco);
            break;
        case 16:
            if (!strcmp(comando, "stack"))
                exibirPilhas(disco);
    }
    return endAtual;
}
