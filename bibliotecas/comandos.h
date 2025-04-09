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
        if (i != 0 && !(i % 10))
            printf("\n");
        if (disco[i].bad)
            printf("[%d: B]  ", i);
        else if (blocoLivre(disco, i)) {
            printf("[%d: F]  ", i);
        } else
            printf("[%d: A]  ", i);
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
    printf("Espaco livre em disco: %d bytes\tEspaco ocupado: %d bytes\n", tamDisco * 10 - ocupado, ocupado);
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
                            if (i == QTDE_INODE_INDIRETO - 1 && j == QTDE_INODE_INDIRETO - 1 && k == QTDE_INODE_INDIRETO
                                - 1)
                                if (disco[disco[endSimples].inodeIndireto.endInd[k]].inode.permissao[0] != '\0')
                                    listar(disco, disco[endSimples].inodeIndireto.endInd[k]);
                                else {
                                    for (l = 2; l < disco[disco[endSimples].inodeIndireto.endInd[k]].dir.TL; l++)
                                        if (!dirVazio(disco[endSimples]))
                                            printf("%s     ",
                                                   disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                   nome);
                                    printf("\n");
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

void rmdir(Bloco *disco, int endEntrada, char *nomeDir) {
    int end, pos, i = 0, inodeDir;

    if (buscaArquivo(disco, endEntrada, nomeDir, &pos, &end) == -1)
        printf("rmdir: falhou em remover '%s': Diretorio inexistente\n", nomeDir);
    else {
        inodeDir = disco[end].dir.arquivo[pos].endInode;
        if (disco[disco[end].dir.arquivo[pos].endInode].inode.permissao[0] == 'd') {
            if (dirVazio(disco[disco[inodeDir].inode.endDireto[0]])) {
                i = pos;
                while (i < disco[end].dir.TL - 1) {
                    strcpy(disco[end].dir.arquivo[i].nome, disco[end].dir.arquivo[i + 1].nome);
                    disco[end].dir.arquivo[i].endInode = disco[end].dir.arquivo[i + 1].endInode;
                    i++;
                }
                disco[disco[disco[end].dir.arquivo[pos].endInode].inode.endDireto[0]].dir.TL = 0;
                disco[end].dir.TL--;
                pushBlocoLivre(disco, disco[inodeDir].inode.endDireto[0]);
                pushBlocoLivre(disco, inodeDir);
            } else
                printf("rmdir: falhou em remover '%s': Diretorio nao vazio\n", nomeDir);
        } else
            printf("rmdir: falhou em remover '%s': Nao e um diretorio\n", nomeDir);
    }
}

void listarAtributos(Bloco *disco, int end) {
    int i, j, k, l, endInode, endSimples, endDuplo, endTriplo;

    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != endNulo()) {
        for (j = 2; j < disco[disco[end].inode.endDireto[i]].dir.TL; j++)
            if (!dirVazio(disco[disco[end].inode.endDireto[i]])) {
                endInode = disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode;
                printf("%s\t%d\t%s\t%s\t%s\t%s",
                       disco[endInode].inode.permissao, disco[endInode].inode.contadorLink,
                       disco[endInode].inode.proprietario, disco[endInode].inode.grupo,
                       disco[endInode].inode.ultimaAlteracao,
                       disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome);
                if (disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.permissao[0] == 'l')
                    printf(" -> %s",
                           disco[disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.endDireto[0]]
                           .softLink.caminho);
                printf("\n");
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
                    printf("%s\t%d\t%s\t%s\t%s\t%s",
                           disco[endInode].inode.permissao, disco[endInode].inode.contadorLink,
                           disco[endInode].inode.proprietario, disco[endInode].inode.grupo,
                           disco[endInode].inode.ultimaAlteracao,
                           disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].
                           nome);
                    if (disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode].inode.permissao[
                            0] == 'l')
                        printf(" -> %s",
                               disco[disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode].
                                   inode.endDireto[0]].softLink.caminho);
                    printf("\n");
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
                            printf("%s\t%d\t%s\t%s\t%s\t%s",
                                   disco[endInode].inode.permissao, disco[endInode].inode.contadorLink,
                                   disco[endInode].inode.proprietario, disco[endInode].inode.grupo,
                                   disco[endInode].inode.ultimaAlteracao,
                                   disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].nome);
                            if (disco[disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].endInode].inode.
                                permissao[0] == 'l')
                                printf(" -> %s",
                                       disco[disco[disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].
                                           endInode].inode.endDireto[0]].softLink.caminho);
                            printf("\n");
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
                            if (i == QTDE_INODE_INDIRETO - 1 && j == QTDE_INODE_INDIRETO - 1 && k == QTDE_INODE_INDIRETO
                                - 1)
                                if (disco[disco[endSimples].inodeIndireto.endInd[k]].inode.permissao[0] != '\0')
                                    listarAtributos(disco, disco[endSimples].inodeIndireto.endInd[k]);
                                else
                                    for (l = 2; l < disco[disco[endSimples].inodeIndireto.endInd[k]].dir.TL; l++)
                                        if (!dirVazio(disco[endSimples])) {
                                            endInode = disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                    endInode;
                                            printf(
                                                "%s\t%d\t%s\t%s\t%s\t%s",
                                                disco[endInode].inode.permissao, disco[endInode].inode.contadorLink,
                                                disco[endInode].inode.proprietario, disco[endInode].inode.grupo,
                                                disco[endInode].inode.ultimaAlteracao,
                                                disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].nome);
                                            if (disco[disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                    endInode].inode.permissao[0] == 'l')
                                                printf(" -> %s",
                                                       disco[disco[disco[disco[endSimples].inodeIndireto.endInd[k]].dir.
                                                           arquivo[l].endInode].inode.endDireto[0]].softLink.caminho);
                                            printf("\n");
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

/*void arvore(Bloco *disco, int end, int nivel, int *vet) {
    int i = 1, j, k;
    char espaco[400] = "", traco[6], curva[12], te[12];

#ifdef __linux__
    strcpy(traco, "│\t");
    strcpy(curva, "└── ");
    strcpy(te, "├── ");
#else
    traco[0] = 179;
    traco[1] = ' ';
    traco[2] = ' ';
    traco[3] = '\t';
    traco[4] = '\0';
    traco[5] = '\0';
    curva[0] = 192;
    curva[1] = 196;
    curva[2] = 196;
    curva[3] = ' ';
    curva[4] = '\0';
    te[0] = 195;
    te[1] = 196;
    te[2] = 196;
    te[3] = ' ';
    te[4] = '\0';
#endif

    if (disco[disco[end].inode.endDireto[0]].dir.TL == 3)
        vet[nivel] = 1; // 1 significa o último diretorio!
    while (i < nivel) {
        if (vet[i] == 0)
            //strcat(espaco, traco);
            strcat(espaco, "|  \t");
        else
#ifdef __linux__
            strcat(espaco, "   \t");
#else
            strcat(espaco, "   \t");
#endif
        i++;
    }
    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != endNulo()) {
        for (j = 2; j < disco[disco[end].inode.endDireto[i]].dir.TL; j++) {
            k = nivel * 4 - 4;
            espaco[k] = '\0';
            if (j + 1 == disco[disco[end].inode.endDireto[i]].dir.TL && i < QTDE_INODE_DIRETO && disco[end].inode.
                endDireto[i + 1] == endNulo()) {
                vet[nivel] = 1;
                strcat(espaco, "`-- ");
                //strcat(espaco, curva);
            } else
                strcat(espaco, "|-- ");
                //strcat(espaco, te);
            printf("%s%s\n", espaco, disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome);
            arvore(disco, disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode, nivel + 1, vet);
        }
        i++;
    }
}*/

void arvore(Bloco *disco, int end, int nivel, int *vet) {
    int i = 1, j, k;
    char espaco[400] = "", traco[10], curva[12], te[12];

#ifdef __linux__
    strcpy(traco, "│\t");
    strcpy(curva, "└── ");
    strcpy(te, "├── ");
#else
    traco[0] = 179;
    traco[1] = ' ';
    traco[2] = ' ';
    traco[3] = ' ';
    traco[4] = '\0';
    traco[5] = '\0';
    curva[0] = 192;
    curva[1] = 196;
    curva[2] = 196;
    curva[3] = ' ';
    curva[4] = '\0';
    te[0] = 195;
    te[1] = 196;
    te[2] = 196;
    te[3] = ' ';
    te[4] = '\0';
#endif

    if (disco[disco[end].inode.endDireto[0]].dir.TL == 3)
        vet[nivel] = 1; // 1 significa o último diretorio!
    while (i < nivel) {
        if (vet[i] == 0)
            strcat(espaco, traco);
        else
            strcat(espaco, "    ");
        i++;
    }
    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != endNulo()) {
        for (j = 2; j < disco[disco[end].inode.endDireto[i]].dir.TL; j++) {
            k = nivel * 4 - 4;
            espaco[k] = '\0';
            if (j + 1 == disco[disco[end].inode.endDireto[i]].dir.TL && i < QTDE_INODE_DIRETO && disco[end].inode.
                endDireto[i + 1] == endNulo()) {
                vet[nivel] = 1;
                strcat(espaco, curva);
            } else
                strcat(espaco, te);
            printf("%s", espaco);
            if (disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.permissao[0] == 'd')
                printf(AZUL);
            else if (disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.permissao[0] == 'l')
                printf(CIANO);
            printf("%s\n%s", disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome, RESET);
            arvore(disco, disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode, nivel + 1, vet);
        }
        i++;
    }
}

void exibirArvore(Bloco *disco, int raiz) {
    int i, vet[100];

    vet[0] = 1;
    for (i = 1; i < 100; i++)
        vet[i] = 0;
    printf("/.\n");
    arvore(disco, raiz, 1, vet);
}

void chmod(Bloco *disco, int endDir, char *nomeArq, char *permUsuario, char *tipoPerm, char tipo) {
    int pos, endArq, i, j;
    char busca, novaPerm[11], data[30];

    busca = buscaArquivo(disco, endDir, nomeArq, &pos, &endArq);

    if (busca != -1) {
        strcpy(novaPerm, disco[disco[endArq].dir.arquivo[pos].endInode].inode.permissao);
        switch (tipo) {
            case '+':
                i = strlen(permUsuario) - 1;
                while (i >= 0) {
                    switch (permUsuario[i]) {
                        case 'u':
                            j = strlen(tipoPerm) - 1;
                            while (j >= 0) {
                                switch (tipoPerm[j]) {
                                    case 'r':
                                        novaPerm[1] = 'r';
                                        break;
                                    case 'w':
                                        novaPerm[2] = 'w';
                                        break;
                                    case 'x':
                                        novaPerm[3] = 'x';
                                }
                                j--;
                            }
                            break;
                        case 'g':
                            j = strlen(tipoPerm) - 1;
                            while (j >= 0) {
                                switch (tipoPerm[j]) {
                                    case 'r':
                                        novaPerm[4] = 'r';
                                        break;
                                    case 'w':
                                        novaPerm[5] = 'w';
                                        break;
                                    case 'x':
                                        novaPerm[6] = 'x';
                                }
                                j--;
                            }
                            break;
                        case 'o':
                            j = strlen(tipoPerm) - 1;
                            while (j >= 0) {
                                switch (tipoPerm[j]) {
                                    case 'r':
                                        novaPerm[7] = 'r';
                                        break;
                                    case 'w':
                                        novaPerm[8] = 'w';
                                        break;
                                    case 'x':
                                        novaPerm[9] = 'x';
                                }
                                j--;
                            }
                            break;
                    }
                    i--;
                }
                break;
            case '-':
                i = strlen(permUsuario) - 1;
                while (i >= 0) {
                    switch (permUsuario[i]) {
                        case 'u':
                            j = strlen(tipoPerm) - 1;
                            while (j >= 0) {
                                switch (tipoPerm[j]) {
                                    case 'r':
                                        novaPerm[1] = '-';
                                        break;
                                    case 'w':
                                        novaPerm[2] = '-';
                                        break;
                                    case 'x':
                                        novaPerm[3] = '-';
                                }
                                j--;
                            }
                            break;
                        case 'g':
                            j = strlen(tipoPerm) - 1;
                            while (j >= 0) {
                                switch (tipoPerm[j]) {
                                    case 'r':
                                        novaPerm[4] = '-';
                                        break;
                                    case 'w':
                                        novaPerm[5] = '-';
                                        break;
                                    case 'x':
                                        novaPerm[6] = '-';
                                }
                                j--;
                            }
                            break;
                        case 'o':
                            j = strlen(tipoPerm) - 1;
                            while (j >= 0) {
                                switch (tipoPerm[j]) {
                                    case 'r':
                                        novaPerm[7] = '-';
                                        break;
                                    case 'w':
                                        novaPerm[8] = '-';
                                        break;
                                    case 'x':
                                        novaPerm[9] = '-';
                                }
                                j--;
                            }
                    }
                    i--;
                }
                break;
        }
        horarioLocal(data);
        strcpy(disco[disco[endArq].dir.arquivo[pos].endInode].inode.ultimaAlteracao, data);
        strcpy(disco[disco[endArq].dir.arquivo[pos].endInode].inode.permissao, novaPerm);
    } else
        printf("Arquivo nao encontrado\n");
}

char validarBlocosArquivo(char *comando, char *nomeArq) {
    int i = 7, j = 0;

    while (i < strlen(comando))
        nomeArq[j++] = comando[i++];
    nomeArq[j] = '\0';
    if (i == strlen(comando))
        return 1;
    return 0;
}

void listarBlocos(Bloco *disco, int end) {
    int i, j, k, endSimples, endDuplo, endTriplo;

    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != endNulo()) {
        printf("[%d] ", disco[end].inode.endDireto[i]);
        if (disco[disco[end].inode.endDireto[i]].bad)
            printf("- Bad\n");
        else
            printf("- Ok\n");
        i++;
    }
    i = 0;
    if (disco[end].inode.endSimplesIndireto != endNulo()) {
        endSimples = disco[end].inode.endSimplesIndireto;
        while (i < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[i] != endNulo()) {
            printf("[%d] ", disco[endSimples].inodeIndireto.endInd[i]);
            if (disco[disco[endSimples].inodeIndireto.endInd[i]].bad)
                printf("- Bad\n");
            else
                printf("- Ok\n");
            i++;
        }
        if (disco[end].inode.endDuploIndireto != endNulo()) {
            i = 0;
            endDuplo = disco[end].inode.endDuploIndireto;
            while (i < QTDE_INODE_INDIRETO && disco[endDuplo].inodeIndireto.endInd[i] != endNulo()) {
                j = 0;
                endSimples = disco[endDuplo].inodeIndireto.endInd[i];
                while (j < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[j] != endNulo()) {
                    printf("[%d] ", disco[endSimples].inodeIndireto.endInd[j]);
                    if (disco[disco[endSimples].inodeIndireto.endInd[j]].bad)
                        printf("- Bad\n");
                    else
                        printf("- Ok\n");
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
                            if (i == QTDE_INODE_INDIRETO - 1 && j == QTDE_INODE_INDIRETO - 1 && k == QTDE_INODE_INDIRETO
                                - 1)
                                if (disco[disco[endSimples].inodeIndireto.endInd[k]].inode.permissao[0] != '\0')
                                    listarBlocos(disco, disco[endSimples].inodeIndireto.endInd[k]);
                                else {
                                    printf("[%d] ", disco[endSimples].inodeIndireto.endInd[k]);
                                    if (disco[disco[endSimples].inodeIndireto.endInd[k]].bad)
                                        printf("- Bad\n");
                                    else
                                        printf("- Ok\n");
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

void mostrarBlocosArquivo(Bloco *disco, int endAtual, char *nomeArq) {
    int endEntrada, pos;

    if (buscaArquivo(disco, endAtual, nomeArq, &pos, &endEntrada) != -1) {
        printf("Blocos em uso do arquivo %s\n", nomeArq);
        listarBlocos(disco, disco[endEntrada].dir.arquivo[pos].endInode);
    } else
        printf("Arquivo %s nao encontrado\n", nomeArq);
}

char validarUnlink(char *comando, char *nomeArq) {
    char tipo = -1;
    int i, j;

    if (comando[7] == '-' && comando[8] == 's')
        tipo = 1;
    else if (comando[7] == '-' && comando[8] == 'h')
        tipo = 2;

    i = 10;
    j = 0;
    while (i < strlen(comando))
        nomeArq[j++] = comando[i++];
    nomeArq[j] = '\0';
    if (i == strlen(comando))
        return tipo;
    return -1;
}

char linkValido(char *comando, char *origem, char *destino) {
    char tipo = 0;
    int i = 8, j;

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

char validarRemocaoArquivo(char *comando, char *nomeArquivo) {
    int i = 3, j = 0;

    while (i < strlen(comando) && comando[i] != ' ')
        nomeArquivo[j++] = comando[i++];
    nomeArquivo[j] = '\0';
    if (i == strlen(comando))
        return 1;
    return 0;
}

char validarAlteracaoPermissao(char *comando, char *usuario, char *tipo, char *nomeArquivo) {
    int i, j;
    char valido = 0;

    if (comando[6] != '-' && comando[6] != '+')
        valido = 0;
    else {
        i = 7;
        j = 0;
        while (i < strlen(comando) && (comando[i] == 'u' || comando[i] == 'g' || comando[i] == 'o'))
            usuario[j++] = comando[i++];
        usuario[j] = '\0';
        if (i < strlen(comando)) {
            i++;
            j = 0;
            while (i < strlen(comando) && (comando[i] == 'r' || comando[i] == 'w' || comando[i] == 'x'))
                tipo[j++] = comando[i++];
            tipo[j] = '\0';

            i++;
            j = 0;
            while (i < strlen(comando) && comando[i] != ' ')
                nomeArquivo[j++] = comando[i++];
            nomeArquivo[j] = '\0';
            if (i == strlen(comando))
                valido = 1;
            else
                valido = 0;
        } else
            valido = 0;
    }

    return valido;
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

int buscaEntradaDiretorio(Bloco *disco, int raiz, int endAtual, char *comando, char *usuario) {
    char caminho[50], entrada[15] = "", caminhoUsuario[20] = "/home/", caminhoTotalAux[50];
    int i = 0, j = 0, busca, pos, novoEnd = endAtual, endBusca;

    while (i < strlen(comando))
        caminho[j++] = comando[i++];
    caminho[j] = '\0';
    strcat(caminhoUsuario, usuario);

    if (strlen(caminho) > 0) {
        if (!strcmp(caminho, "/"))
            novoEnd = raiz;
        else {
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
                    printf("bash: cd: Arquivo ou diretorio inexistente\n");
                else {
                    novoEnd = disco[endAtual].dir.arquivo[pos].endInode;
                }
            } else {
                if (!strcmp(caminho, ".")) {
                    novoEnd = disco[disco[endAtual].inode.endDireto[0]].dir.arquivo[0].endInode;
                } else {
                    if (!strcmp(caminho, "..")) {
                        novoEnd = disco[disco[endAtual].inode.endDireto[0]].dir.arquivo[1].endInode;
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
                            printf("bash: cd: Arquivo ou diretorio inexistente\n");
                        else {
                            novoEnd = disco[endAtual].dir.arquivo[pos].endInode;
                        }
                    }
                }
            }
        }
    }
    if (bad(disco[novoEnd])) {
        novoEnd = -1;
        printf("Erro: Diretorio corrompido\n");
        return novoEnd;
    }
    if (disco[novoEnd].inode.permissao[0] == 'd')
        return novoEnd;
    return -1;
}

void listarLinks(Bloco *disco, int end) {
    int i, j, k, l, endSimples, endDuplo, endTriplo;

    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != endNulo()) {
        for (j = 2; j < disco[disco[end].inode.endDireto[i]].dir.TL; j++)
            if (!dirVazio(disco[disco[end].inode.endDireto[i]])) {
                if (disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.permissao[0] == 'l') {
                    printf("%s - 'link -s' - inode [%d] - bloco [%d] - caminho: %s\n",
                           disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome,
                           disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode,
                           disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.endDireto[i],
                           disco[disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.endDireto[0]]
                           .softLink.caminho
                    );
                } else if (disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.contadorLink > 1) {
                    printf("%s - 'link -h' - inode [%d] - contador: %d\n",
                           disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome,
                           disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode,
                           disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.contadorLink
                    );
                }
            }
        i++;
    }
    i = 0;
    if (disco[end].inode.endSimplesIndireto != endNulo()) {
        endSimples = disco[end].inode.endSimplesIndireto;
        while (i < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[i] !=
               endNulo()) {
            for (j = 2; j < disco[disco[endSimples].inodeIndireto.endInd[i]].dir.TL; j++)
                if (!dirVazio(disco[disco[endSimples].inodeIndireto.endInd[i]])) {
                    if (disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.permissao[0] == 'l') {
                        printf("%s - 'link -s' - inode [%d] - bloco [%d] - caminho: %s\n",
                               disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].nome,
                               disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode,
                               disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode].inode.
                               endDireto[i],
                               disco[disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode].
                                   inode.endDireto
                                   [0]].softLink.caminho
                        );
                    } else if (disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode].inode.
                               contadorLink >
                               1) {
                        printf("%s - 'link -h' - inode [%d] - contador: %d\n",
                               disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].nome,
                               disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode,
                               disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode].inode.
                               contadorLink
                        );
                    }
                }
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
                        if (!dirVazio(disco[disco[endSimples].inodeIndireto.endInd[j]])) {
                            if (disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.permissao[0]
                                == 'l') {
                                printf("%s - 'link -s' - inode [%d] - bloco [%d] - caminho: %s\n",
                                       disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].nome,
                                       disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode,
                                       disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode].
                                       inode.endDireto[i],
                                       disco[disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].
                                               endInode].inode.endDireto
                                           [0]].softLink.caminho
                                );
                            } else if (disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode].
                                       inode.contadorLink >
                                       1) {
                                printf("%s - 'link -h' - inode [%d] - contador: %d\n",
                                       disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].nome,
                                       disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode,
                                       disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode].
                                       inode.contadorLink
                                );
                            }
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
                            if (i == QTDE_INODE_INDIRETO - 1 && j == QTDE_INODE_INDIRETO - 1 && k == QTDE_INODE_INDIRETO
                                - 1)
                                if (disco[disco[endSimples].inodeIndireto.endInd[k]].inode.permissao[0] != '\0')
                                    listarLinks(disco, disco[endSimples].inodeIndireto.endInd[k]);
                                else
                                    for (l = 2; l < disco[disco[endSimples].inodeIndireto.endInd[k]].dir.TL; l++)
                                        if (!dirVazio(disco[endSimples])) {
                                            if (disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].
                                                inode.
                                                permissao[0]
                                                == 'l') {
                                                printf("%s - 'link -s' - inode [%d] - bloco [%d] - caminho: %s\n",
                                                       disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                       nome,
                                                       disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                       endInode,
                                                       disco[disco[disco[endSimples].inodeIndireto.endInd[k]].dir.
                                                           arquivo[l].
                                                           endInode].
                                                       inode.endDireto[i],
                                                       disco[disco[disco[disco[endSimples].inodeIndireto.endInd[k]].dir.
                                                               arquivo[
                                                                   l].
                                                               endInode].inode.endDireto
                                                           [0]].softLink.caminho
                                                );
                                            } else if (disco[disco[disco[endSimples].inodeIndireto.endInd[k]].dir.
                                                           arquivo[l].
                                                           endInode].
                                                       inode.contadorLink >
                                                       1) {
                                                printf("%s - 'link -h' - inode [%d] - contador: %d\n",
                                                       disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                       nome,
                                                       disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                       endInode,
                                                       disco[disco[disco[endSimples].inodeIndireto.endInd[k]].dir.
                                                           arquivo[l].
                                                           endInode].
                                                       inode.contadorLink
                                                );
                                            }
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

void visualizarLinks(Bloco *disco, int raiz, int tamDisco) {
    int endAtual = raiz;

    while (endAtual < tamDisco) {
        if (disco[endAtual].inode.permissao[0] == 'd')
            listarLinks(disco, endAtual);
        endAtual++;
    }
}

void listarArquivos(Bloco *disco, int end) {
    int i, j, k, l, endSimples, endDuplo, endTriplo;

    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != endNulo()) {
        for (j = 2; j < disco[disco[end].inode.endDireto[i]].dir.TL; j++)
            if (!dirVazio(disco[disco[end].inode.endDireto[i]])) {
                if (disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.permissao[0] == 'd') {
                    printf("Diretorio: %s - inode [%d]\n",
                           disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome,
                           disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode
                    );
                } else {
                    printf("Arquivo: %s - inode [%d]\n",
                           disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome,
                           disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode
                    );
                }
            }
        i++;
    }
    i = 0;
    if (disco[end].inode.endSimplesIndireto != endNulo()) {
        endSimples = disco[end].inode.endSimplesIndireto;
        while (i < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[i] !=
               endNulo()) {
            for (j = 2; j < disco[disco[endSimples].inodeIndireto.endInd[i]].dir.TL; j++)
                if (!dirVazio(disco[disco[endSimples].inodeIndireto.endInd[i]])) {
                    if (disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode].inode.permissao[
                            0] == 'd') {
                        printf("Diretorio: %s - inode [%d]\n",
                               disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].nome,
                               disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode
                        );
                    } else {
                        printf("Arquivo: %s - inode [%d]\n",
                               disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].nome,
                               disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode
                        );
                    }
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
                            if (disco[disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].endInode].inode.
                                permissao[0] == 'd') {
                                printf("Diretorio: %s - inode [%d]\n",
                                       disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].nome,
                                       disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].endInode
                                );
                            } else {
                                printf("Arquivo: %s - inode [%d]\n",
                                       disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].nome,
                                       disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].endInode
                                );
                            }
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
                            if (i == QTDE_INODE_INDIRETO - 1 && j == QTDE_INODE_INDIRETO - 1 && k == QTDE_INODE_INDIRETO
                                - 1)
                                if (disco[disco[endSimples].inodeIndireto.endInd[k]].inode.permissao[0] != '\0')
                                    listar(disco, disco[endSimples].inodeIndireto.endInd[k]);
                                else
                                    for (l = 2; l < disco[disco[endSimples].inodeIndireto.endInd[k]].dir.TL; l++)
                                        if (!dirVazio(disco[disco[endSimples].inodeIndireto.endInd[k]])) {
                                            if (disco[disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                    endInode].
                                                inode.permissao[0] == 'd') {
                                                printf("Diretorio: %s - inode [%d]\n",
                                                       disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                       nome,
                                                       disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                       endInode
                                                );
                                            } else {
                                                printf("Arquivo: %s - inode [%d]\n",
                                                       disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                       nome,
                                                       disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                       endInode
                                                );
                                            }
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

void visualizarArquivosDir(Bloco *disco, int raiz, int tamDisco) {
    int endAtual = raiz;

    printf("Diretorio: raiz - inode[%d]\n", raiz);
    while (endAtual < tamDisco) {
        if (disco[endAtual].inode.permissao[0] == 'd' || disco[endAtual].inode.permissao[0] == 'a')
            listarArquivos(disco, endAtual);
        endAtual++;
    }
}

void contarBlocosPerdidos(Bloco *disco, int end, int *cont) {
    int i, j, k, endSimples, endDuplo, endTriplo, contAnt = *cont;
    char bad = 0;

    if (disco[end].bad == 1)
        bad = 1;
    if (bad)
        (*cont)++;
    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != endNulo()) {
        if (disco[disco[end].inode.endDireto[i]].bad == 1)
            bad = 1;
        if (bad)
            (*cont)++;
        i++;
    }
    i = 0;
    if (disco[end].inode.endSimplesIndireto != endNulo()) {
        endSimples = disco[end].inode.endSimplesIndireto;
        if (disco[endSimples].bad == 1)
            bad = 1;
        if (bad)
            (*cont)++;
        while (i < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[i] != endNulo()) {
            if (disco[disco[endSimples].inodeIndireto.endInd[i]].bad == 1)
                bad = 1;
            if (bad)
                (*cont)++;
            i++;
        }
        if (disco[end].inode.endDuploIndireto != endNulo()) {
            i = 0;
            endDuplo = disco[end].inode.endDuploIndireto;
            if (disco[endDuplo].bad == 1)
                bad = 1;
            if (bad)
                (*cont)++;
            while (i < QTDE_INODE_INDIRETO && disco[endDuplo].inodeIndireto.endInd[i] != endNulo()) {
                j = 0;
                endSimples = disco[endDuplo].inodeIndireto.endInd[i];
                if (disco[endSimples].bad == 1)
                    bad = 1;
                if (bad)
                    (*cont)++;
                while (j < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[j] != endNulo()) {
                    if (disco[disco[endSimples].inodeIndireto.endInd[j]].bad == 1)
                        bad = 1;
                    if (bad)
                        (*cont)++;
                    j++;
                }
                i++;
            }
            if (disco[end].inode.endTriploIndireto != endNulo()) {
                i = 0;
                endTriplo = disco[end].inode.endTriploIndireto;
                if (disco[endTriplo].bad == 1)
                    bad = 1;
                if (bad)
                    (*cont)++;
                while (i < QTDE_INODE_INDIRETO && disco[endTriplo].inodeIndireto.endInd[i] != endNulo()) {
                    j = 0;
                    endDuplo = disco[endTriplo].inodeIndireto.endInd[i];
                    if (disco[endDuplo].bad == 1)
                        bad = 1;
                    if (bad)
                        (*cont)++;
                    while (j < QTDE_INODE_INDIRETO && disco[endDuplo].inodeIndireto.endInd[j] != endNulo()) {
                        k = 0;
                        endSimples = disco[endDuplo].inodeIndireto.endInd[j];
                        if (disco[endSimples].bad == 1)
                            bad = 1;
                        if (bad)
                            (*cont)++;
                        while (k < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[k] != endNulo()) {
                            if (i == QTDE_INODE_INDIRETO - 1 && j == QTDE_INODE_INDIRETO - 1 && k == QTDE_INODE_INDIRETO
                                - 1)
                                if (disco[disco[endSimples].inodeIndireto.endInd[k]].inode.permissao[0] != '\0')
                                    contarBlocosPerdidos(disco, disco[endSimples].inodeIndireto.endInd[k], &*cont);
                                else {
                                    if (disco[disco[endSimples].inodeIndireto.endInd[k]].bad == 1)
                                        bad = 1;
                                    if (bad)
                                        (*cont)++;
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
    if ((*cont) > contAnt)
        (*cont)--;
}

int perdidos(Bloco *disco, int raiz, int tamDisco) {
    int endAtual = raiz, cont = 0;

    while (endAtual < tamDisco) {
        if (disco[endAtual].inode.permissao[0] != '\0') {
            contarBlocosPerdidos(disco, endAtual, &cont); // conta a partir do i-node
        }
        endAtual++;
    }
    return cont;
}

void criarLinkFisico(Bloco *disco, int raiz, char *usuario, int endDir, char *origem, char *destino) {
    int endOrigem, i = 0, j = 0, k, pos, endEntrada, endDestino, inodeOrigem;
    char busca, caminhoOrigem[50], caminhoDestino[50], nomeArquivoOrigem[15], nomeArquivoDestino[15], aux[50] = "";

    strcpy(caminhoOrigem, origem);
    strcpy(caminhoDestino, destino);
    if (origem[0] != '/' && origem[0] != '.') {
        strcat(aux, "./");
        strcat(aux, origem);
        strcpy(origem, aux);
    }
    aux[0] = '\0';
    if (destino[0] != '/' && destino[0] != '.') {
        strcat(aux, "./");
        strcat(aux, destino);
        strcpy(destino, aux);
    }
    if (origem[0] == '/' || origem[0] == '.') {
        i = strlen(origem) - 1;
        while (i >= 0 && origem[i] != '/')
            i--;
        k = i;
        i++;
        while (i < strlen(origem))
            nomeArquivoOrigem[j++] = origem[i++];
        nomeArquivoOrigem[j] = '\0';
        if (k != 1)
            origem[k] = '\0';
        else
            origem[k + 1] = '\0';
        if (origem[0] == '\0')
            strcpy(origem, "/");
    } else
        strcpy(nomeArquivoOrigem, origem);

    if (destino[0] == '/' || destino[0] == '.') {
        i = strlen(destino) - 1;
        while (i >= 0 && destino[i] != '/')
            i--;
        k = i;
        i++;
        j = 0;
        while (i < strlen(destino))
            nomeArquivoDestino[j++] = destino[i++];
        nomeArquivoDestino[j] = '\0';
        if (k != 1)
            destino[k] = '\0';
        else
            destino[k + 1] = '\0';
        if (destino[0] == '\0')
            strcpy(destino, "/");
    } else
        strcpy(nomeArquivoDestino, destino);

    endOrigem = buscaEntradaDiretorio(disco, raiz, endDir, origem, usuario);
    busca = buscaArquivo(disco, endOrigem, nomeArquivoOrigem, &pos, &endEntrada);
    if (busca != -1) {
        endDestino = buscaEntradaDiretorio(disco, raiz, endDir, destino, usuario);
        if (endDestino != -1) {
            adicionarEntrada(disco, endDestino, usuario, nomeArquivoDestino, 'l', 1, origem);
            inodeOrigem = disco[endEntrada].dir.arquivo[pos].endInode;
            disco[inodeOrigem].inode.contadorLink++;
            busca = buscaArquivo(disco, endDestino, nomeArquivoDestino, &pos, &endEntrada);
            pushBlocoLivre(disco, disco[disco[endEntrada].dir.arquivo[pos].endInode].inode.endDireto[0]);
            pushBlocoLivre(disco, disco[endEntrada].dir.arquivo[pos].endInode);
            disco[endEntrada].dir.arquivo[pos].endInode = inodeOrigem;
        } else
            printf("ln: falha ao criar link simbolico '%s': Arquivo ou diretorio inexistente\n", caminhoDestino);
    } else
        printf("ln: falha ao criar link simbolico '%s': Arquivo ou diretorio inexistente\n", caminhoOrigem);
}

void remanejarCaminho(char *caminho) {
    char aux[50];
    int i, j;

    strcpy(aux, caminho);
    i = 2;
    j = 0;
    while (i < strlen(aux))
        caminho[j++] = aux[i++];
    caminho[j] = '\0';
}

void caminhoAbsoluto(char *caminho, char *pwd) {
    char aux[50];

    strcpy(aux, caminho);
    if (aux[0] == '.' && aux[1] == '/')
        remanejarCaminho(aux);
    strcpy(caminho, pwd);
    strcat(caminho, "/");
    strcat(caminho, aux);
}

void tratarCaminho(char *origem, char *destino, char *origemFinal, char *pwd) {
    int i, j;

    if (origem[0] != '/')
        caminhoAbsoluto(origem, pwd);
    if (destino[0] != '/')
        caminhoAbsoluto(destino, pwd);

    i = 1;
    while (i < strlen(origem) && i < strlen(destino) && origem[i] == destino[i])
        i++;
    if (i > 1 && origem[i - 1] == '/' && i == strlen(destino)) {
        j = 0;
        strcpy(origemFinal, "");
        while (i < strlen(origem))
            origemFinal[j++] = origem[i++];
        origemFinal[j] = '\0';
    } else if (i > 1 && origem[i] == '/' && i == strlen(destino)) {
        i++;
        j = 0;
        strcpy(origemFinal, "");
        while (i < strlen(origem))
            origemFinal[j++] = origem[i++];
        origemFinal[j] = '\0';
    } else
        strcpy(origemFinal, origem);
}

void criarLinkSimbolico(Bloco *disco, int raiz, char *usuario, int endDir, char *origem, char *destino,
                        char *caminhoAbs) {
    int i = 0, j = 0, k, endDestino, pos, endAux;
    char caminhoDestino[50], aux[50], nomeArquivoDestino[15], caminho[50];

    strcpy(caminhoDestino, destino);
    aux[0] = '\0';
    if (destino[0] != '/' && destino[0] != '.') {
        strcat(aux, "./");
        strcat(aux, destino);
        strcpy(destino, aux);
    }
    if (destino[0] == '/' || destino[0] == '.') {
        i = strlen(destino) - 1;
        while (i >= 0 && destino[i] != '/')
            i--;
        k = i;
        i++;
        while (i < strlen(destino))
            nomeArquivoDestino[j++] = destino[i++];
        nomeArquivoDestino[j] = '\0';
        if (k != 1)
            destino[k] = '\0';
        else
            destino[k + 1] = '\0';
        if (destino[0] == '\0')
            strcpy(destino, "/");
    } else
        strcpy(nomeArquivoDestino, destino);

    endDestino = buscaEntradaDiretorio(disco, raiz, endDir, destino, usuario);

    if (endDestino != -1) {
        tratarCaminho(origem, destino, caminho, caminhoAbs);
        if (buscaArquivo(disco, endDir, nomeArquivoDestino, &pos, &endAux) == -1)
            adicionarEntrada(disco, endDestino, usuario, nomeArquivoDestino, 'l', 1, caminho);
        else
            printf("ln: falha ao criar link simbolico '%s': Nome do arquivo ja existe\n", nomeArquivoDestino);
    } else
        printf("ln: falha ao criar link simbolico '%s': Arquivo ou diretorio inexistente\n", caminhoDestino);
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
    char busca, nomeArquivo[15], data[30];
    int i = 3, j = 0, end, pos;

    while (i < strlen(comando) && comando[i] != ' ')
        nomeArquivo[j++] = comando[i++];
    nomeArquivo[j] = '\0';

    if (i == strlen(comando) && strlen(nomeArquivo) < 15) {
        busca = buscaArquivo(disco, endDir, nomeArquivo, &pos, &end);
        if (busca != -1) {
            if (disco[disco[end].dir.arquivo[pos].endInode].inode.permissao[0] == '-') {
                if (disco[disco[end].dir.arquivo[pos].endInode].bad != 1 && !corrompido(
                        disco, disco[disco[end].dir.arquivo[pos].endInode])) {
                    printf("Arquivo %s visualizado\n", disco[end].dir.arquivo[pos].nome);
                    horarioLocal(data);
                    strcpy(disco[disco[end].dir.arquivo[pos].endInode].inode.ultimoAcesso, data);
                } else
                    printf("O arquivo %s esta corrompido\n", nomeArquivo);
            } else {
                if (disco[disco[end].dir.arquivo[pos].endInode].inode.permissao[0] == 'l')
                    if (disco[disco[end].dir.arquivo[pos].endInode].bad != 1 && !corrompido(
                            disco, disco[disco[end].dir.arquivo[pos].endInode])) {
                        printf("Arquivo %s visualizado\n", disco[end].dir.arquivo[pos].nome);
                        horarioLocal(data);
                        strcpy(disco[disco[end].dir.arquivo[pos].endInode].inode.ultimoAcesso, data);
                    } else
                        printf("O arquivo %s esta corrompido\n", nomeArquivo);
                else
                    printf("O arquivo %s e um diretorio\n", disco[end].dir.arquivo[pos].nome);
            }
        } else
            printf("Arquivo nao encontrado\n");
    } else
        printf("Arquivo nao encontrado\n");
}

void aumentarBlocos(Bloco *disco, int endInode, int quant) {
    int i, j, k, endSimples, endDuplo, endTriplo;

    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[endInode].inode.endDireto[i] != endNulo())
        i++;
    while (i < QTDE_INODE_DIRETO && quant > 0) {
        disco[endInode].inode.endDireto[i++] = popBlocoLivre(disco);
        quant--;
    }
    if (quant > 0) {
        if (disco[endInode].inode.endSimplesIndireto == endNulo())
            disco[endInode].inode.endSimplesIndireto = criarInodeInd(disco);
        i = 0;
        endSimples = disco[endInode].inode.endSimplesIndireto;
        while (i < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[i] != endNulo())
            i++;
        while (i < QTDE_INODE_INDIRETO && quant > 0) {
            disco[endSimples].inodeIndireto.endInd[i++] = popBlocoLivre(disco);
            disco[endSimples].inodeIndireto.TL++;
            quant--;
        }
        if (quant > 0) {
            if (disco[endInode].inode.endDuploIndireto == endNulo())
                disco[endInode].inode.endDuploIndireto = criarInodeInd(disco);
            i = 0;
            endDuplo = disco[endInode].inode.endDuploIndireto;
            while (i < QTDE_INODE_INDIRETO && quant > 0) {
                j = 0;
                if (disco[endDuplo].inodeIndireto.endInd[i] == endNulo())
                    disco[endDuplo].inodeIndireto.endInd[i] = criarInodeInd(disco);
                endSimples = disco[endDuplo].inodeIndireto.endInd[i];
                while (j < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[j] != endNulo())
                    j++;
                while (j < QTDE_INODE_INDIRETO && quant > 0) {
                    disco[endSimples].inodeIndireto.endInd[j++] = popBlocoLivre(disco);
                    disco[endSimples].inodeIndireto.TL++;
                    quant--;
                }
                i++;
            }
            if (quant > 0) {
                if (disco[endInode].inode.endTriploIndireto == endNulo())
                    disco[endInode].inode.endTriploIndireto = criarInodeInd(disco);
                i = 0;
                endTriplo = disco[endInode].inode.endTriploIndireto;
                while (i < QTDE_INODE_INDIRETO && quant > 0) {
                    j = 0;
                    if (disco[endTriplo].inodeIndireto.endInd[i] == endNulo())
                        disco[endTriplo].inodeIndireto.endInd[i] = criarInodeInd(disco);
                    endDuplo = disco[endTriplo].inodeIndireto.endInd[i];
                    while (j < QTDE_INODE_INDIRETO && quant > 0) {
                        k = 0;
                        if (disco[endDuplo].inodeIndireto.endInd[j] == endNulo())
                            disco[endDuplo].inodeIndireto.endInd[j] = criarInodeInd(disco);
                        endSimples = disco[endDuplo].inodeIndireto.endInd[j];
                        while (k < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[k] !=
                               endNulo())
                            k++;
                        if (i == QTDE_INODE_INDIRETO - 1 && j == QTDE_INODE_INDIRETO - 1 && k == QTDE_INODE_INDIRETO) {
                            if (disco[endSimples].inodeIndireto.endInd[k - 1] != endNulo()) {
                                pushBlocoLivre(disco, disco[endSimples].inodeIndireto.endInd[k - 1]);
                                disco[endSimples].inodeIndireto.endInd[k - 1] = criarInode(
                                    disco, disco[endInode].inode.proprietario,
                                    'a', 1, 1, "");
                                aumentarBlocos(disco, disco[endSimples].inodeIndireto.endInd[k - 1], quant + 1);
                            }
                        } else {
                            while (k < QTDE_INODE_INDIRETO && quant > 0) {
                                disco[endSimples].inodeIndireto.endInd[k++] = popBlocoLivre(disco);
                                disco[endSimples].inodeIndireto.TL++;
                                quant--;
                            }
                        }
                        j++;
                    }
                    i++;
                }
            }
        }
    }
}

void diminuirBlocos(Bloco *disco, int endInode, int *quant) {
    int i, j, k, endSimples, endDuplo, endTriplo;

    i = QTDE_INODE_INDIRETO - 1;
    endTriplo = disco[endInode].inode.endTriploIndireto;
    if (endTriplo != endNulo())
        if (disco[disco[disco[endTriplo].inodeIndireto.endInd[4]].inodeIndireto.endInd[4]].inodeIndireto.endInd[4]) {
            diminuirBlocos(
                disco,
                disco[disco[disco[endTriplo].inodeIndireto.endInd[4]].inodeIndireto.endInd[4]].inodeIndireto.endInd[4],
                &*quant);
            pushBlocoLivre(
                disco,
                disco[disco[disco[endTriplo].inodeIndireto.endInd[4]].inodeIndireto.endInd[4]].inodeIndireto.endInd[4]);
            disco[disco[disco[endTriplo].inodeIndireto.endInd[4]].inodeIndireto.endInd[4]].inodeIndireto.endInd[4] =
                    popBlocoLivre(disco);
        }
    if (endTriplo != endNulo()) {
        while (i >= 0 && *quant > 0) {
            j = QTDE_INODE_INDIRETO - 1;
            if (disco[endTriplo].inodeIndireto.endInd[i] != endNulo()) {
                endDuplo = disco[endTriplo].inodeIndireto.endInd[i];
                while (j >= 0 && *quant > 0) {
                    k = QTDE_INODE_INDIRETO - 1;
                    if (disco[endDuplo].inodeIndireto.endInd[j] != endNulo()) {
                        endSimples = disco[endDuplo].inodeIndireto.endInd[j];
                        if (disco[endSimples].inodeIndireto.endInd[k] != endNulo()) {
                            while (k != 0 && disco[endSimples].inodeIndireto.endInd[k] == endNulo())
                                k++;
                            while (k >= 0 && *quant > 0) {
                                pushBlocoLivre(disco, disco[endSimples].inodeIndireto.endInd[k]);
                                disco[endSimples].inodeIndireto.endInd[k--] = -1;
                                disco[endSimples].inodeIndireto.TL--;
                                (*quant)--;
                            }
                        }
                        if (disco[endSimples].inodeIndireto.TL == 0) {
                            pushBlocoLivre(disco, disco[endDuplo].inodeIndireto.endInd[j]);
                            disco[endDuplo].inodeIndireto.endInd[j] = -1;
                            disco[endDuplo].inodeIndireto.TL--;
                        }
                    }
                    j--;
                }
                if (disco[endDuplo].inodeIndireto.TL == 0) {
                    pushBlocoLivre(disco, disco[endTriplo].inodeIndireto.endInd[i]);
                    disco[endTriplo].inodeIndireto.endInd[i] = -1;
                    disco[endTriplo].inodeIndireto.TL--;
                }
            }
            i--;
        }
        if (disco[endTriplo].inodeIndireto.TL == 0) {
            pushBlocoLivre(disco, disco[endInode].inode.endTriploIndireto);
            disco[endInode].inode.endTriploIndireto = -1;
        }
    }
    if (*quant > 0) {
        i = QTDE_INODE_INDIRETO - 1;
        endDuplo = disco[endInode].inode.endDuploIndireto;
        if (endDuplo != endNulo()) {
            while (i >= 0 && *quant > 0) {
                j = QTDE_INODE_INDIRETO - 1;
                if (disco[endDuplo].inodeIndireto.endInd[i] != endNulo()) {
                    endSimples = disco[endDuplo].inodeIndireto.endInd[i];
                    if (disco[endSimples].inodeIndireto.endInd[j] != endNulo()) {
                        while (j != 0 && disco[endSimples].inodeIndireto.endInd[j] == endNulo())
                            j++;
                        while (j >= 0 && *quant > 0) {
                            pushBlocoLivre(disco, disco[endSimples].inodeIndireto.endInd[j]);
                            disco[endSimples].inodeIndireto.endInd[j--] = -1;
                            disco[endSimples].inodeIndireto.TL--;
                            (*quant)--;
                        }
                    }
                    if (disco[endSimples].inodeIndireto.TL == 0) {
                        pushBlocoLivre(disco, disco[endDuplo].inodeIndireto.endInd[i]);
                        disco[endDuplo].inodeIndireto.endInd[i] = -1;
                        disco[endDuplo].inodeIndireto.TL--;
                    }
                }
                i--;
            }
            if (disco[endDuplo].inodeIndireto.TL == 0) {
                pushBlocoLivre(disco, disco[endInode].inode.endDuploIndireto);
                disco[endInode].inode.endDuploIndireto = -1;
            }
        }
        if (*quant > 0) {
            i = QTDE_INODE_INDIRETO - 1;
            endSimples = disco[endInode].inode.endSimplesIndireto;
            if (endSimples != endNulo()) {
                while (i != 0 && disco[endSimples].inodeIndireto.endInd[i] == endNulo())
                    i--;
                while (i >= 0 && *quant > 0) {
                    pushBlocoLivre(disco, disco[endSimples].inodeIndireto.endInd[i]);
                    disco[endSimples].inodeIndireto.endInd[i--] = -1;
                    disco[endSimples].inodeIndireto.TL--;
                    (*quant)--;
                }
                if (disco[endSimples].inodeIndireto.TL == 0) {
                    pushBlocoLivre(disco, disco[endInode].inode.endSimplesIndireto);
                    disco[endInode].inode.endSimplesIndireto = -1;
                }
            }
            if (*quant > 0) {
                i = QTDE_INODE_DIRETO - 1;
                while (i != 0 && disco[endInode].inode.endDireto[i] == endNulo())
                    i--;
                while (i >= 0 && quant > 0) {
                    pushBlocoLivre(disco, disco[endInode].inode.endDireto[i]);
                    disco[endInode].inode.endDireto[i--] = -1;
                    (*quant)--;
                }
            }
        }
    }
}

void rm(Bloco *disco, int endEntrada, char *nomeArq) {
    int end, pos, i = 0, inodeArq, tam, endExcluir;

    if (buscaArquivo(disco, endEntrada, nomeArq, &pos, &end) == -1)
        printf("rm: falhou em remover '%s': Arquivo ou diretorio inexistente\n", nomeArq);
    else {
        inodeArq = disco[end].dir.arquivo[pos].endInode;
        if (disco[inodeArq].inode.permissao[0] == 'd') {
            if (dirVazio(disco[disco[inodeArq].inode.endDireto[0]])) {
                i = pos;
                while (i < disco[end].dir.TL - 1) {
                    strcpy(disco[end].dir.arquivo[i].nome, disco[end].dir.arquivo[i + 1].nome);
                    disco[end].dir.arquivo[i].endInode = disco[end].dir.arquivo[i + 1].endInode;
                    i++;
                }
                disco[disco[disco[end].dir.arquivo[pos].endInode].inode.endDireto[0]].dir.TL = 0;
                disco[end].dir.TL--;
                pushBlocoLivre(disco, disco[inodeArq].inode.endDireto[0]);
                pushBlocoLivre(disco, inodeArq);
            } else
                printf("rm: falhou em remover '%s': Diretorio nao vazio\n", nomeArq);
        } else {
            i = pos;
            endExcluir = disco[end].dir.arquivo[i].endInode;
            while (i < disco[end].dir.TL) {
                strcpy(disco[end].dir.arquivo[i].nome, disco[end].dir.arquivo[i + 1].nome);
                disco[end].dir.arquivo[i].endInode = disco[end].dir.arquivo[i + 1].endInode;
                i++;
            }
            disco[end].dir.TL--;
            if (disco[inodeArq].inode.permissao[0] == 'l') {
                pushBlocoLivre(disco, disco[inodeArq].inode.endDireto[0]);
                pushBlocoLivre(disco, inodeArq);
            } else {
                disco[endExcluir].inode.contadorLink--;
                if (disco[endExcluir].inode.contadorLink == 0) {
                    tam = (int) (ceil)((float) disco[inodeArq].inode.tamanho / (float) 10);
                    diminuirBlocos(disco, inodeArq, &tam);
                    pushBlocoLivre(disco, inodeArq);
                }
            }
            inicializarBloco(&disco[endExcluir]);
        }
    }
}

void removerLinkFisico(Bloco *disco, int raiz, char *usuario, int endDir, char *destino) {
    int i = 0, j = 0, k, endDestino, pos, endEntrada, inode;
    char caminhoArquivo[50], aux[50], nomeArquivo[15];

    strcpy(caminhoArquivo, destino);
    aux[0] = '\0';
    if (destino[0] != '/' && destino[0] != '.') {
        strcat(aux, "./");
        strcat(aux, destino);
        strcpy(destino, aux);
    }
    if (destino[0] == '/' || destino[0] == '.') {
        i = strlen(destino) - 1;
        while (i >= 0 && destino[i] != '/')
            i--;
        k = i;
        i++;
        while (i < strlen(destino))
            nomeArquivo[j++] = destino[i++];
        nomeArquivo[j] = '\0';
        if (k != 1)
            destino[k] = '\0';
        else
            destino[k + 1] = '\0';
        if (destino[0] == '\0')
            strcpy(destino, "/");
    } else
        strcpy(nomeArquivo, destino);

    endDestino = buscaEntradaDiretorio(disco, raiz, endDir, destino, usuario);
    if (endDestino != -1) {
        if (buscaArquivo(disco, endDestino, nomeArquivo, &pos, &endEntrada)) {
            inode = disco[endEntrada].dir.arquivo[pos].endInode;
            if (disco[inode].inode.contadorLink > 1) {
                i = pos;
                while (i < disco[endEntrada].dir.TL - 1) {
                    strcpy(disco[endEntrada].dir.arquivo[i].nome, disco[endEntrada].dir.arquivo[i + 1].nome);
                    disco[endEntrada].dir.arquivo[i].endInode = disco[endEntrada].dir.arquivo[i + 1].endInode;
                    i++;
                }
                disco[endEntrada].dir.TL--;
                disco[inode].inode.contadorLink--;
                //diminuirBlocos(disco, inode, 1);
                //pushBlocoLivre(disco, inode);
            }
        }
    } else
        printf("ln: falha ao criar link simbolico '%s': Arquivo ou diretorio inexistente", caminhoArquivo);
}

void removerLinkSimbolico(Bloco *disco, int raiz, char *usuario, int endDir, char *destino) {
    int i = 0, j = 0, k, endDestino, pos, endEntrada, inode, tam = 1;
    char caminhoArquivo[50], aux[50], nomeArquivo[15];

    strcpy(caminhoArquivo, destino);
    aux[0] = '\0';
    if (destino[0] != '/' && destino[0] != '.') {
        strcat(aux, "./");
        strcat(aux, destino);
        strcpy(destino, aux);
    }
    if (destino[0] == '/' || destino[0] == '.') {
        i = strlen(destino) - 1;
        while (i >= 0 && destino[i] != '/')
            i--;
        k = i;
        i++;
        while (i < strlen(destino))
            nomeArquivo[j++] = destino[i++];
        nomeArquivo[j] = '\0';
        if (k != 1)
            destino[k] = '\0';
        else
            destino[k + 1] = '\0';
        if (destino[0] == '\0')
            strcpy(destino, "/");
    } else
        strcpy(nomeArquivo, destino);

    endDestino = buscaEntradaDiretorio(disco, raiz, endDir, destino, usuario);
    if (endDestino != -1) {
        if (buscaArquivo(disco, endDestino, nomeArquivo, &pos, &endEntrada)) {
            inode = disco[endEntrada].dir.arquivo[pos].endInode;
            if (disco[inode].inode.contadorLink == 1) {
                i = pos;
                while (i < disco[endEntrada].dir.TL - 1) {
                    strcpy(disco[endEntrada].dir.arquivo[i].nome, disco[endEntrada].dir.arquivo[i + 1].nome);
                    disco[endEntrada].dir.arquivo[i].endInode = disco[endEntrada].dir.arquivo[i + 1].endInode;
                    i++;
                }
                disco[endEntrada].dir.TL--;
                diminuirBlocos(disco, inode, &tam);
                pushBlocoLivre(disco, inode);
            }
        }
    } else
        printf("ln: falha ao criar link simbolico '%s': Arquivo ou diretorio inexistente", caminhoArquivo);
}

int maiorArquivoPossivel(Bloco *disco) {
    int qtdeBlocos, cont = 0, i = 0, j, k;

    qtdeBlocos = qtdeBlocosLivres(disco);
    while (qtdeBlocos > 0) {
        i = 0;
        qtdeBlocos--; // i-node
        while (qtdeBlocos > 0 && i < 5) {
            i++;
            qtdeBlocos--;
            cont++;
        }
        if (qtdeBlocos > 0) {
            qtdeBlocos--; // i-node indireto simples
            i = 0;
            while (qtdeBlocos > 0 && i < 5) {
                i++;
                qtdeBlocos--;
                cont++;
            }
        }
        if (qtdeBlocos > 0) {
            i = 0;
            qtdeBlocos--; // i-node indireto duplo
            while (qtdeBlocos > 0 && i < 5) {
                j = 0;
                qtdeBlocos--; // i-node indireto simples
                while (qtdeBlocos > 0 && j < 5) {
                    j++;
                    qtdeBlocos--;
                    cont++;
                }
                i++;
            }
        }
        if (qtdeBlocos > 0) {
            i = 0;
            qtdeBlocos--; // i-node indireto duplo
            while (qtdeBlocos > 0 && i < 5) {
                j = 0;
                qtdeBlocos--; // i-node indireto simples
                while (qtdeBlocos > 0 && j < 5) {
                    k = 0;
                    qtdeBlocos--;
                    while (qtdeBlocos > 0 && k < 5) {
                        qtdeBlocos--;
                        cont++;
                        k++;
                    }
                    j++;
                }
                i++;
            }
        }
    }
    return cont;
}

void touch(Bloco *disco, int end, int raiz, char *usuario, char *nomeArq, int tam) {
    int pos, endArq, tamAnt, diferenca, qtdeBlocos, restoUltBloco, novaQuant, i, j = 0, k, endDestino;
    char data[30], caminhoDestino[50], aux[20], nomeArquivoDestino[15];

    strcpy(caminhoDestino, nomeArq);
    aux[0] = '\0';
    if (nomeArq[0] != '/' && nomeArq[0] != '.') {
        strcat(aux, "./");
        strcat(aux, nomeArq);
        strcpy(nomeArq, aux);
    }
    if (nomeArq[0] == '/' || nomeArq[0] == '.') {
        i = strlen(nomeArq) - 1;
        while (i >= 0 && nomeArq[i] != '/')
            i--;
        k = i;
        i++;
        while (i < strlen(nomeArq))
            nomeArquivoDestino[j++] = nomeArq[i++];
        nomeArquivoDestino[j] = '\0';
        if (k != 1)
            nomeArq[k] = '\0';
        else
            nomeArq[k + 1] = '\0';
        if (nomeArq[0] == '\0')
            strcpy(nomeArq, "/");
    } else
        strcpy(nomeArquivoDestino, nomeArq);

    endDestino = buscaEntradaDiretorio(disco, raiz, end, nomeArq, usuario);
    if (endDestino != -1)
        if (buscaArquivo(disco, end, nomeArquivoDestino, &pos, &endArq) == -1) {
            if (qtdeBlocosNecessarios((int) ((float) tam / (float) 10)) < qtdeBlocosLivres(disco))
                adicionarEntrada(disco, endDestino, usuario, nomeArquivoDestino, 'a', tam, "");
            else
                printf("Espaco em disco insuficiente!\n");
        } else {
            if (qtdeBlocosNecessarios((int) ((float) tam / (float) 10)) < qtdeBlocosLivres(disco)) {
                horarioLocal(data);
                strcpy(disco[disco[endArq].dir.arquivo[pos].endInode].inode.ultimaAlteracao, data);
                tamAnt = disco[disco[endArq].dir.arquivo[pos].endInode].inode.tamanho;
                diferenca = tam - tamAnt;
                restoUltBloco = tamAnt % 10;
                disco[disco[endArq].dir.arquivo[pos].endInode].inode.tamanho = tam;
                if (diferenca > 0) {
                    qtdeBlocos = ceil((((float) tam - (float) tamAnt) + (float) restoUltBloco) / 10);
                    if (qtdeBlocosNecessarios(qtdeBlocos) < qtdeBlocosLivres(disco))
                        aumentarBlocos(disco, disco[endArq].dir.arquivo[pos].endInode, qtdeBlocos);
                } else {
                    qtdeBlocos = ceil((float) tamAnt / 10);
                    novaQuant = ceil((float) tam / 10);
                    diferenca = qtdeBlocos - novaQuant;
                    diminuirBlocos(disco, disco[endArq].dir.arquivo[pos].endInode, &diferenca);
                }
            } else
                printf("Espaco em disco insuficiente!\n");
        }
}

void mkdir(Bloco *disco, int end, char *usuario, char *nomeDir) {
    int pos, endArq;

    if (buscaArquivo(disco, end, nomeDir, &pos, &endArq) == -1) {
        if ((float) 1 / (float) 10 < qtdeBlocosLivres(disco))
            adicionarEntrada(disco, end, usuario, nomeDir, 'd', 1, "");
        else
            printf("Espaco em disco insuficiente!\n");
    } else
        printf("mkdir: nao foi possível criar o diretorio “%s”: Arquivo existe\n", nomeDir);
}

int navegar(Bloco *disco, int raiz, int endUsuario, int endAtual, char *comando, char *usuario,
            char *caminhoTotal) {
    char caminho[50], entrada[15] = "", caminhoUsuario[20] = "/home/", caminhoTotalAux[50], data[30];
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
                    printf("bash: cd: Arquivo ou diretorio inexistente\n");
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
                            printf("bash: cd: Arquivo ou diretorio inexistente\n");
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
        printf("Erro: Diretorio corrompido\n");
        return novoEnd;
    }
    if (disco[novoEnd].inode.permissao[0] == 'd') {
        horarioLocal(data);
        strcpy(disco[novoEnd].inode.ultimoAcesso, data);
        return novoEnd;
    }
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
    else if (!strcmp(comandoPrincipal, "maior") || !strcmp(comandoPrincipal, "maiorp"))
        c = 17;
    else if (!strcmp(comandoPrincipal, "status"))
        c = 18;
    else if (!strcmp(comandoPrincipal, "perdidos"))
        c = 19;
    else if (!strcmp(comandoPrincipal, "arquivos"))
        c = 20;
    else if (!strcmp(comandoPrincipal, "links"))
        c = 21;
    else if (!strcmp(comandoPrincipal, "exit") || !strcmp(comandoPrincipal, "poweroff"))
        c = 22;
    return c;
}

int executarComando(Bloco *disco, char *usuario, int raiz, int endUsuario, int end, char *comando, char c, int tamDisco,
                    char *caminho) {
    int i = 0, tam, endAtual = end, blocosPerdidos;
    char nomeArq[50], origem[50], destino[50], tipoPerm[4], usuarioPerm[4], tipo, tipoLink;

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
                printf("Opcao nao existente para o comando mkdir\n");
            break;
        case 2:
            if (validarRemocao(comando, nomeArq))
                rmdir(disco, endAtual, nomeArq);
            break;
        case 3:
            if (validarRemocaoArquivo(comando, nomeArq))
                rm(disco, endAtual, nomeArq);
            break;
        case 4:
            endAtual = navegar(disco, raiz, endUsuario, endAtual, comando, usuario, caminho);
            if (endAtual == -1) {
                printf("bash: cd: %s: Nao e um diretorio\n", caminho);
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
                printf("Nao foi possível criar o link '%s' para '%s': Arquivo ou diretorio inexistente\n", origem,
                       destino);
            else {
                if (tipoLink == 1)
                    criarLinkSimbolico(disco, raiz, usuario, end, origem, destino, caminho);
                else
                    criarLinkFisico(disco, raiz, usuario, end, origem, destino);
            }
            break;
        case 6:
            setBad(disco, comando, tamDisco);
            break;
        case 7:
            tipoLink = validarUnlink(comando, nomeArq);
            if (tipoLink == -1)
                printf("Nao foi possível remover o link '%s' para '%s': Arquivo ou diretorio inexistente\n", origem,
                       destino);
            else if (tipoLink == 1)
                removerLinkSimbolico(disco, raiz, usuario, end, nomeArq);
            else
                removerLinkFisico(disco, raiz, usuario, end, nomeArq);
            break;
        case 8:
            if (validarCriacaoArq(comando, nomeArq, &tam))
                if (strlen(nomeArq) <= 49)
                    touch(disco, end, raiz, usuario, nomeArq, tam);
                else
                    printf("Nome de arquivo muito longo\n");
            else
                printf("Opcao nao existente para o comando touch\n");
            break;
        case 9:
            if (!strcmp(comando, "df"))
                livresOcupadosBytes(disco, tamDisco);
            break;
        case 10:
            if (!strcmp(comando, "clear"))
                limparTela();
            else
                printf("Opcao nao existente para o comando clear\n");
            break;
        case 11:
            if (validarAlteracaoPermissao(comando, usuarioPerm, tipoPerm, nomeArq)) {
                tipo = comando[6];
                chmod(disco, end, nomeArq, usuarioPerm, tipoPerm, tipo);
            }
            break;
        case 12:
            visualizarArquivo(disco, end, comando);
            break;
        case 13:
            if (!strcmp(comando, "pwd"))
                printf("%s\n", caminho);
            else
                printf("Opcao nao existente para o comando pwd\n");
            break;
        case 14:
            if (!strcmp(comando, "tree"))
                exibirArvore(disco, raiz);
            break;
        case 15:
            if (!strcmp(comando, "blocks"))
                mostrarBlocos(disco, tamDisco);
            else if (validarBlocosArquivo(comando, nomeArq))
                mostrarBlocosArquivo(disco, end, nomeArq);
            break;
        case 16:
            if (!strcmp(comando, "stack"))
                exibirPilhas(disco);
            break;
        case 17:
            if (!strcmp(comando, "maior"))
                printf("O maior arquivo possui %d blocos\n", maior(disco, raiz, tamDisco));
            else if (!strcmp(comando, "maiorp")) {
                tam = maiorArquivoPossivel(disco);
                printf("Maior arquivo possível: %d blocos (%d bytes)\n", tam, tam * 10);
            }
            break;
        case 18:
            if (!strcmp(comando, "status"))
                status(disco, raiz, tamDisco);
            break;
        case 19:
            if (!strcmp(comando, "perdidos")) {
                blocosPerdidos = perdidos(disco, raiz, tamDisco);
                printf("Blocos perdidos: %d - tamanho em bytes: %d\n", blocosPerdidos, blocosPerdidos * 10);
            }
            break;
        case 20:
            if (!strcmp(comando, "arquivos"))
                visualizarArquivosDir(disco, raiz, tamDisco);
            break;
        case 21:
            if (!strcmp(comando, "links"))
                visualizarLinks(disco, raiz, tamDisco);
            break;
    }
    return endAtual;
}
