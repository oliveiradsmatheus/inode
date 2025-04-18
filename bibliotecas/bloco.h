#define CABECA_LISTA_BL 0
#define MAXIMO_BL 9
#define PERMISSAO_ARQUIVO "-rw-r--r--"
#define PERMISSAO_DIRETORIO "drwxr-xr-x"
#define PERMISSAO_LINK "lrwxrwxrwx"
#define QTDE_MAX_DIR 12
#define TAM_MAX_NOME 15
#define QTDE_INODE_DIRETO 5
#define QTDE_INODE_INDIRETO 5

struct inode {
    char permissao[11];
    char proprietario[20];
    char grupo[20];
    int contadorLink;
    char criacao[30];
    char ultimoAcesso[30];
    char ultimaAlteracao[30];
    int tamanho;
    int endDireto[QTDE_INODE_DIRETO];
    int endSimplesIndireto;
    int endDuploIndireto;
    int endTriploIndireto;
};

typedef struct inode Inode;

struct inodeindireto {
    int endInd[QTDE_INODE_INDIRETO];
    int TL;
};

typedef struct inodeindireto InodeInd;

struct softlink {
    char caminho[50];
};

typedef struct softlink SoftLink;

struct entrada {
    char nome[TAM_MAX_NOME];
    int endInode;
};

typedef struct entrada Entrada;

struct diretorio {
    Entrada arquivo[QTDE_MAX_DIR]; // pai, o próprio diretório e 10 arquivos.
    int TL;
};

typedef struct diretorio Diretorio;

struct listaBlocosLivres {
    int topo;
    int end[10];
    int endProxLista;
};

typedef struct listaBlocosLivres ListaBlocosLivres;

struct bloco {
    char bad;
    Inode inode;
    InodeInd inodeIndireto;
    Diretorio dir;
    ListaBlocosLivres listaBlocosLivres;
    SoftLink softLink;
};

typedef struct bloco Bloco;

int criarInode(Bloco *disco, char *usuario, char tipoArq, int tamanho, int endPai, char *caminho);

void horarioLocal(char *data) {
    time_t t;
    char dia[4], mes[4];
    int i;

    t = time(NULL);
    strcpy(data, ctime(&t));
    data[strlen(data) - 1] = '\0';

    for (i = 0; i < 3; i++) {
        dia[i] = data[i];
        mes[i] = data[i + 4];
    }

    if (!strcmp(dia, "Mon"))
        strcpy(dia, "Seg");
    else if (!strcmp(dia, "Tue"))
        strcpy(dia, "Ter");
    else if (!strcmp(dia, "Wed"))
        strcpy(dia, "Qua");
    else if (!strcmp(dia, "Thu"))
        strcpy(dia, "Qui");
    else if (!strcmp(dia, "Fri"))
        strcpy(dia, "Sex");
    else if (!strcmp(dia, "Sat"))
        strcpy(dia, "Sab");
    else if (!strcmp(dia, "Sun"))
        strcpy(dia, "Dom");

    if (!strcmp(mes, "Feb"))
        strcpy(mes, "Fev");
    else if (!strcmp(mes, "Apr"))
        strcpy(mes, "Abr");
    else if (!strcmp(mes, "May"))
        strcpy(mes, "Mai");
    else if (!strcmp(mes, "Aug"))
        strcpy(mes, "Ago");
    else if (!strcmp(mes, "Sep"))
        strcpy(mes, "Set");
    else if (!strcmp(mes, "Oct"))
        strcpy(mes, "Out");
    else if (!strcmp(mes, "Dec"))
        strcpy(mes, "Dez");

    for (i = 0; i < 3; i++) {
        data[i] = dia[i];
        data[i + 4] = mes[i];
    }
}

int endNulo(void) {
    return -1;
}

char blocoLivre(Bloco *disco, int endBloco) {
    int end = 0, endProx = disco[0].listaBlocosLivres.endProxLista, i;
    char livre = 0;
    while (end != endNulo() && !livre) {
        i = 0;
        while (i <= disco[end].listaBlocosLivres.topo && !livre) {
            if (disco[end].listaBlocosLivres.end[i] == endBloco)
                livre = 1;
            i++;
        }
        end = endProx;
        endProx = disco[end].listaBlocosLivres.endProxLista;
    }
    return livre;
}

char arquivo(Bloco *disco, int end) {
    if (!blocoLivre(disco, end) && disco[end].bad == 0 && disco[end].dir.TL == 0 && disco[end].inode.permissao[0] ==
        '\0' && disco[end].listaBlocosLivres.topo ==
        endNulo() && disco[end].softLink.caminho[0] == '\0')
        return 1;
    return 0;
}

void inicializarBloco(Bloco *bloco) {
    (*bloco).bad = 0;
    (*bloco).dir.TL = 0;
    (*bloco).inode.permissao[0] = '\0';
    (*bloco).listaBlocosLivres.topo = endNulo();
    (*bloco).softLink.caminho[0] = '\0';
}

void inicializarDisco(Bloco *disco, int tamDisco) {
    int i;
    for (i = 0; i < tamDisco; i++)
        inicializarBloco(&disco[i]);
}

void ligarListaBlocosLivres(Bloco *bloco, int qtde) {
    int i;
    for (i = 0; i < qtde; i++)
        bloco[i].listaBlocosLivres.endProxLista = i + 1;
    bloco[i - 1].listaBlocosLivres.endProxLista = endNulo();
}

char bad(Bloco bloco) {
    return bloco.bad;
}

char corrompido(Bloco *disco, Bloco blocoInode) {
    char corr = 0;
    int i, j, k, indSimples, indDuplo, indTriplo;

    for (i = 0; i < QTDE_INODE_DIRETO && blocoInode.inode.endDireto[i] != endNulo() && !corr; i++)
        if (bad(disco[blocoInode.inode.endDireto[i]]))
            corr = 1;

    indSimples = blocoInode.inode.endSimplesIndireto;
    if (disco[indSimples].bad == 1)
        corr = 1;
    if (indSimples != endNulo()) {
        for (i = 0; i < QTDE_INODE_INDIRETO && disco[indSimples].inodeIndireto.endInd[i] != endNulo() && !corr; i
             ++)
            if (bad(disco[disco[indSimples].inodeIndireto.endInd[i]]))
                corr = 1;

        indDuplo = blocoInode.inode.endDuploIndireto;
        if (disco[indDuplo].bad == 1)
            corr = 1;
        if (indDuplo != endNulo()) {
            for (i = 0; i < QTDE_INODE_INDIRETO && disco[indDuplo].inodeIndireto.endInd[i] != endNulo() && !corr;
                 i++) {
                indSimples = disco[indDuplo].inodeIndireto.endInd[i];
                if (disco[indSimples].bad == 1)
                    corr = 1;
                for (j = 0; j < QTDE_INODE_INDIRETO && disco[indSimples].inodeIndireto.endInd[j] != endNulo() && !
                            corr; j++)
                    if (bad(disco[disco[indSimples].inodeIndireto.endInd[j]]))
                        corr = 1;
            }
            indTriplo = blocoInode.inode.endTriploIndireto;
            if (disco[indTriplo].bad == 1)
                corr = 1;
            if (indTriplo != endNulo())
                for (i = 0; i < QTDE_INODE_INDIRETO && disco[indTriplo].inodeIndireto.endInd[i] != endNulo() && !
                            corr; i++) {
                    indDuplo = disco[indTriplo].inodeIndireto.endInd[i];
                    if (disco[indDuplo].bad == 1)
                        corr = 1;
                    for (j = 0; j < QTDE_INODE_INDIRETO && disco[indDuplo].inodeIndireto.endInd[j] != endNulo() && !
                                corr; j++) {
                        indSimples = disco[indDuplo].inodeIndireto.endInd[j];
                        if (disco[indSimples].bad == 1)
                            corr = 1;
                        for (k = 0; k < QTDE_INODE_INDIRETO && disco[indSimples].inodeIndireto.endInd[k] != endNulo() &&
                                    !corr; k++)
                            if (i == QTDE_INODE_INDIRETO - 1 && j == QTDE_INODE_INDIRETO - 1 && k == QTDE_INODE_INDIRETO
                                - 1) {
                                if (disco[disco[indSimples].inodeIndireto.endInd[k]].inode.permissao[0] != '\0')
                                    corr = corrompido(disco, disco[disco[indSimples].inodeIndireto.endInd[k]]);
                            } else if (bad(disco[disco[indSimples].inodeIndireto.endInd[k]]))
                                corr = 1;
                    }
                }
        }
    }
    return corr;
}

char listaVazia(Bloco disco) {
    return disco.listaBlocosLivres.topo == endNulo();
}

char listaCheia(Bloco disco) {
    return disco.listaBlocosLivres.topo == MAXIMO_BL;
}

char fimLista(Bloco disco) {
    return disco.listaBlocosLivres.endProxLista == endNulo();
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

int qtdeBlocosNecessarios(int qtdeBlocos) {
    int i, j, k, cont = 1;
    i = 0;
    while (i < QTDE_INODE_DIRETO && qtdeBlocos > 0) {
        i++;
        cont++;
        qtdeBlocos--;
    }
    if (qtdeBlocos > 0) {
        cont++;
        i = 0;
        while (i < QTDE_INODE_INDIRETO && qtdeBlocos > 0) {
            i++;
            cont++;
            qtdeBlocos--;
        }
        if (qtdeBlocos > 0) {
            cont++;
            i = 0;
            while (i < QTDE_INODE_INDIRETO && qtdeBlocos > 0) {
                j = 0;
                cont++;
                while (j < QTDE_INODE_INDIRETO && qtdeBlocos > 0) {
                    cont++;
                    qtdeBlocos--;
                    j++;
                }
                i++;
            }
            if (qtdeBlocos > 0) {
                cont++;
                i = 0;
                while (i < QTDE_INODE_INDIRETO && qtdeBlocos > 0) {
                    j = 0;
                    cont++;
                    while (j < QTDE_INODE_INDIRETO && qtdeBlocos > 0) {
                        cont++;
                        k = 0;
                        while (k < QTDE_INODE_INDIRETO && qtdeBlocos > 0) {
                            cont++;
                            qtdeBlocos--;
                            k++;
                        }
                        j++;
                    }
                    i++;
                }
            }
        }
    }
    if (qtdeBlocos > 0)
        return cont + qtdeBlocosNecessarios(qtdeBlocos);
    return cont;
}

int qtdeBlocosLivresbkp(Bloco *disco) {
    int qtde = 0, end = CABECA_LISTA_BL;

    while (!fimLista(disco[end])) {
        qtde += disco[end].listaBlocosLivres.topo + 1;
        end = disco[end].listaBlocosLivres.endProxLista;
    }
    qtde += disco[end].listaBlocosLivres.topo + 1;

    return qtde;
}

int qtdeBlocosLivres(Bloco *disco) {
    int qtde = 0, i, endBloco, end = CABECA_LISTA_BL;

    while (!fimLista(disco[end])) {
        for (i = 0; i <= disco[end].listaBlocosLivres.topo; i++) {
            endBloco = disco[end].listaBlocosLivres.end[i];
            if (!bad(disco[endBloco]))
                qtde++;
        }
        end = disco[end].listaBlocosLivres.endProxLista;
    }
    for (i = 0; i <= disco[end].listaBlocosLivres.topo; i++) {
        endBloco = disco[end].listaBlocosLivres.end[i];
        if (!bad(disco[endBloco]))
            qtde++;
    }

    return qtde;
}

int popBlocoLivre(Bloco *disco) {
    int end, endProx;

    end = CABECA_LISTA_BL;
    endProx = disco[end].listaBlocosLivres.endProxLista;
    while (listaVazia(disco[end])) {
        end = endProx;
        endProx = disco[end].listaBlocosLivres.endProxLista;
    }
    if (end != endNulo())
        if (!listaVazia(disco[end])) {
            if (bad(disco[disco[end].listaBlocosLivres.end[disco[end].listaBlocosLivres.topo]])) {
                disco[end].listaBlocosLivres.end[disco[end].listaBlocosLivres.topo--];
                return popBlocoLivre(disco);
            }
            return disco[end].listaBlocosLivres.end[disco[end].listaBlocosLivres.topo--];
        }
    return endNulo();
}

void pushBlocoLivre(Bloco *disco, int bloco) {
    int end, endProx;

    end = CABECA_LISTA_BL;
    endProx = disco[end].listaBlocosLivres.endProxLista;
    while (listaVazia(disco[end]) && !fimLista(disco[end])) {
        end = endProx;
        endProx = disco[end].listaBlocosLivres.endProxLista;
    }
    if (listaCheia(disco[end]))
        end -= 1;
    disco[end].listaBlocosLivres.end[++disco[end].listaBlocosLivres.topo] = bloco;
}

void exibirPilhas(Bloco *disco) {
    int i, j;

    i = CABECA_LISTA_BL;
    while (disco[i].listaBlocosLivres.endProxLista != -1) {
        printf("Pilha %d\nTopo: %d\n", i, disco[i].listaBlocosLivres.topo);
        for (j = 0; j < 10; j++)
            if (j <= disco[i].listaBlocosLivres.topo)
                printf("[   %d   ] \n", disco[i].listaBlocosLivres.end[j]);
            else
                printf("[   --   ] \n");
        printf("End. prox: %d\n", disco[i].listaBlocosLivres.endProxLista);
        i = disco[i].listaBlocosLivres.endProxLista;
    }
    printf("Pilha %d\nTopo: %d\n", i, disco[i].listaBlocosLivres.topo);
    for (j = 0; j < 10; j++)
        if (j <= disco[i].listaBlocosLivres.topo)
            printf("[   %d   ] \n", disco[i].listaBlocosLivres.end[j]);
        else
            printf("[   --   ] \n");
    printf("End. prox: %d\n", disco[i].listaBlocosLivres.endProxLista);
}

void criarListaBlocosLivres(Bloco *disco, int tamDisco) {
    int i, j, qtde, bloco, aux;

    qtde = ceil(((float) tamDisco) / 11);
    ligarListaBlocosLivres(disco, qtde);
    bloco = qtde;
    i = CABECA_LISTA_BL;
    while (i < qtde - 1) {
        for (j = 0; j < 10 && bloco < tamDisco; j++) {
            disco[i].listaBlocosLivres.topo++;
            disco[i].listaBlocosLivres.end[9 - j] = bloco++;
        }
        i++;
    }
    aux = bloco;
    bloco = tamDisco - 1;
    while (bloco >= aux) {
        disco[i].listaBlocosLivres.end[++disco[i].listaBlocosLivres.topo] = bloco--;
    }
    i = CABECA_LISTA_BL;
    while (disco[i].listaBlocosLivres.endProxLista != endNulo())
        i++;
    if (listaVazia(disco[i]))
        disco[i - 1].listaBlocosLivres.endProxLista = endNulo();
}

char dirVazio(Bloco disco) {
    return disco.dir.TL == 2;
}

char dirCheio(Bloco disco) {
    return disco.dir.TL == QTDE_MAX_DIR;
}

int buscaArquivo(Bloco *disco, int endDir, char *nomeArquivo, int *posicao, int *endereco) {
    int end, i, j, k;
    char achou = 0;

    end = 0;
    while (disco[endDir].inode.endDireto[end] != -1 && end < 5 && !achou) {
        i = 0;
        while (i < disco[disco[endDir].inode.endDireto[end]].dir.TL && strcmp(
                   disco[disco[endDir].inode.endDireto[end]].dir.arquivo[i].nome, nomeArquivo) != 0) {
            i++;
        }
        if (i < disco[disco[endDir].inode.endDireto[end]].dir.TL) {
            achou = 1;
            *posicao = i;
            *endereco = disco[endDir].inode.endDireto[end];
        }
        end++;
    }
    if (!achou && disco[endDir].inode.endSimplesIndireto != -1) {
        end = 0;
        while (disco[disco[endDir].inode.endSimplesIndireto].inodeIndireto.endInd[end] != -1 && end < 5 && !achou) {
            i = 0;
            while (i < disco[disco[disco[endDir].inode.endSimplesIndireto].inodeIndireto.endInd[end]].dir.TL && strcmp(
                       disco[disco[disco[endDir].inode.endSimplesIndireto].inodeIndireto.endInd[end]].dir.arquivo[i].
                       nome, nomeArquivo) != 0) {
                i++;
            }
            if (i < disco[disco[disco[endDir].inode.endSimplesIndireto].inodeIndireto.endInd[end]].dir.TL) {
                achou = 1;
                *posicao = i;
                *endereco = disco[disco[endDir].inode.endSimplesIndireto].inodeIndireto.endInd[end];
            }
            end++;
        }
    }
    if (!achou && disco[endDir].inode.endDuploIndireto != -1) {
        end = 0;
        while (disco[disco[endDir].inode.endDuploIndireto].inodeIndireto.endInd[end] != -1 && end < 5 && !achou) {
            j = 0;
            while (disco[disco[disco[endDir].inode.endDuploIndireto].inodeIndireto.endInd[end]].inodeIndireto.endInd[j]
                   != -1 && j < 5 && !achou) {
                i = 0;
                while (i < disco[disco[disco[disco[endDir].inode.endDuploIndireto].inodeIndireto.endInd[end]].
                           inodeIndireto.endInd[j]].dir.TL && strcmp(
                           disco[disco[disco[disco[endDir].inode.endDuploIndireto].inodeIndireto.endInd[end]].
                               inodeIndireto.endInd[j]].dir.arquivo[i].nome, nomeArquivo) != 0) {
                    i++;
                }
                if (i < disco[disco[disco[disco[endDir].inode.endDuploIndireto].inodeIndireto.endInd[end]].inodeIndireto
                        .endInd[j]].dir.TL) {
                    achou = 1;
                    *posicao = i;
                    *endereco = disco[disco[disco[endDir].inode.endDuploIndireto].inodeIndireto.endInd[end]].
                            inodeIndireto.endInd[j];
                }
                j++;
            }
            end++;
        }
    }
    if (!achou && disco[endDir].inode.endTriploIndireto != -1) {
        end = 0;
        while (disco[disco[endDir].inode.endTriploIndireto].inodeIndireto.endInd[end] != -1 && end < 5 && !achou) {
            j = 0;
            while (disco[disco[disco[endDir].inode.endTriploIndireto].inodeIndireto.endInd[end]].inodeIndireto.endInd[j]
                   != -1 && j < 5 && !achou) {
                k = 0;
                while (disco[disco[disco[disco[endDir].inode.endTriploIndireto].inodeIndireto.endInd[end]].inodeIndireto
                           .endInd[j]].inodeIndireto.endInd[k] != -1 && k < 5 && !achou) {
                    i = 0;
                    while (i < disco[disco[disco[disco[disco[endDir].inode.endTriploIndireto].inodeIndireto.endInd[end]]
                               .inodeIndireto.endInd[j]].inodeIndireto.endInd[k]].dir.TL && strcmp(
                               disco[disco[disco[disco[disco[endDir].inode.endTriploIndireto].inodeIndireto.endInd[end]]
                                   .inodeIndireto.endInd[j]].inodeIndireto.endInd[k]].dir.arquivo[i].nome,
                               nomeArquivo) != 0) {
                        i++;
                    }
                    if (i < disco[disco[disco[disco[disco[endDir].inode.endTriploIndireto].inodeIndireto.endInd[end]].
                            inodeIndireto.endInd[j]].inodeIndireto.endInd[k]].dir.TL) {
                        achou = 1;
                        *posicao = i;
                        *endereco = disco[disco[disco[disco[endDir].inode.endTriploIndireto].inodeIndireto.endInd[end]].
                            inodeIndireto.endInd[j]].inodeIndireto.endInd[k];
                    }
                    k++;
                }
                j++;
            }
            end++;
        }
    }
    if (achou)
        return achou;
    return -1;
}

int maior(Bloco *disco, int raiz, int tamDisco) {
    int endAtual = raiz, maior = 0;

    while (endAtual < tamDisco) {
        if (endAtual != endNulo() && disco[endAtual].inode.permissao[0] == '-')
            if (disco[endAtual].inode.tamanho > maior)
                maior = disco[endAtual].inode.tamanho;
        endAtual++;
    }
    return (int) (ceil(((float) maior) / (float) 10));
}

void listarStatus(Bloco *disco, int end) {
    int i, j, k, l, endSimples, endDuplo, endTriplo;

    i = 0;
    while (i < QTDE_INODE_DIRETO && disco[end].inode.endDireto[i] != endNulo()) {
        for (j = 2; j < disco[disco[end].inode.endDireto[i]].dir.TL; j++)
            if (!dirVazio(disco[disco[end].inode.endDireto[i]])) {
                if (disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode].inode.permissao[0] == '-')
                    if (!corrompido(disco, disco[disco[disco[end].inode.endDireto[i]].dir.arquivo[j].endInode]))
                        printf("%s - Íntegro\n", disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome);
                    else
                        printf("%s - %sCorrompido%s\n", disco[disco[end].inode.endDireto[i]].dir.arquivo[j].nome,
                               VERMELHO, RESET);
            }
        i++;
    }
    i = 0;
    if (disco[end].inode.endSimplesIndireto != endNulo()) {
        endSimples = disco[end].inode.endSimplesIndireto;
        while (i < QTDE_INODE_INDIRETO && disco[endSimples].inodeIndireto.endInd[i] !=
               endNulo()) {
            for (j = 2; j < disco[disco[endSimples].inodeIndireto.endInd[i]].dir.TL; j++)
                if (!dirVazio(disco[disco[endSimples].inodeIndireto.endInd[i]]))
                    if (disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode].inode.permissao[
                            0] == '-')
                        if (!corrompido(
                            disco, disco[disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].endInode]))
                            printf("%s - Íntegro\n",
                                   disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].nome);
                        else
                            printf("%s - %sCorrompido%s\n",
                                   disco[disco[endSimples].inodeIndireto.endInd[i]].dir.arquivo[j].nome, VERMELHO,
                                   RESET);
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
                            if (disco[disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].endInode].inode.
                                permissao[0] == '-')
                                if (!corrompido(
                                    disco,
                                    disco[disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].endInode]))
                                    printf("%s - Íntegro\n",
                                           disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].nome);
                                else
                                    printf("%s - %sCorrompido%s\n",
                                           disco[disco[endSimples].inodeIndireto.endInd[j]].dir.arquivo[k].nome,
                                           VERMELHO, RESET);
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
                                if (i == QTDE_INODE_INDIRETO - 1 && j == QTDE_INODE_INDIRETO - 1 && k ==
                                    QTDE_INODE_INDIRETO
                                    - 1)
                                    if (disco[disco[endSimples].inodeIndireto.endInd[k]].inode.permissao[0] != '\0')
                                        listarStatus(disco, disco[endSimples].inodeIndireto.endInd[k]);
                                    else if (!dirVazio(disco[endSimples]))
                                        if (disco[disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[l].
                                                endInode].
                                            inode.permissao[0] == '-')
                                            if (!corrompido(
                                                disco,
                                                disco[disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[
                                                        l].
                                                    endInode]))
                                                printf("%s - Íntegro\n",
                                                       disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[
                                                           l].
                                                       nome);
                                            else
                                                printf("%s - %sCorrompido%s\n",
                                                       disco[disco[endSimples].inodeIndireto.endInd[k]].dir.arquivo[
                                                           l].nome, VERMELHO, RESET);
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

void status(Bloco *disco, int raiz, int tamDisco) {
    int end = raiz;

    while (end < tamDisco) {
        if (disco[end].inode.permissao[0] != '\0')
            listarStatus(disco, end);
        end++;
    }
}

void adicionarArquivo(Bloco *disco, int endDir, char *nomeArquivo, int endInode) {
    int pos, endArq;

    if (buscaArquivo(disco, endDir, nomeArquivo, &pos, &endArq) == -1) {
        disco[endDir].dir.arquivo[disco[endDir].dir.TL].endInode = endInode;
        strcpy(disco[endDir].dir.arquivo[disco[endDir].dir.TL++].nome, nomeArquivo);
    }
}

void setBad(Bloco *disco, char *comando, int tamDisco) {
    char bloco[8];
    int i = 0, j = 0, blocoN;

    while (i < strlen(comando) && comando[i] != 'd')
        i++;
    i++;
    while (i < strlen(comando) && comando[i] == ' ')
        i++;
    while (i < strlen(comando) && comando[i] >= '0' && comando[i] <= '9')
        bloco[j++] = comando[i++];
    bloco[j] = '\0';
    blocoN = atoi(bloco);
    if (blocoN < tamDisco)
        disco[blocoN].bad = 1;
}

void setTam(Bloco *bloco, int end, int tamanho) {
    bloco[end].inode.tamanho = tamanho;
}

char permissaoValida(char *permissao) {
    int i = 0;
    char valida = 1;

    for (i = 0; i < strlen(permissao) && valida; i++) {
        if (i % 3 == 0) {
            if (permissao[i] != 'r' && permissao[i] != '-')
                valida = 0;
        } else if (i % 3 == 1) {
            if (permissao[i] != 'w' && permissao[i] != '-')
                valida = 0;
        } else if (i % 3 == 2)
            if (permissao[i] != 'x' && permissao[i] != '-')
                valida = 0;
    }
    return valida;
}

void alterarPermissao(Bloco *disco, int end, char *permissao) {
    if (permissaoValida(permissao))
        strcpy(disco[end].inode.permissao, permissao);
}

int criarInodeInd(Bloco *disco) {
    int i = 0, bloco;

    bloco = popBlocoLivre(disco);
    if (bloco != -1) {
        for (i = 0; i < QTDE_INODE_INDIRETO; i++)
            disco[bloco].inodeIndireto.endInd[i] = -1;
        disco[bloco].inodeIndireto.TL = 0;
    }

    return bloco;
}

void inserirInodeIS(Bloco *disco, int endInode, int endInodeInd, int *qtBlocos, int inseridoT, char *usuario,
                    char tipoArq) {
    int bloco;

    if (disco[endInodeInd].inodeIndireto.TL < QTDE_INODE_INDIRETO - inseridoT) {
        int inicio = disco[endInodeInd].inodeIndireto.TL;
        while (*qtBlocos > 0 && inicio < QTDE_INODE_INDIRETO - inseridoT) {
            bloco = popBlocoLivre(disco);
            if (bloco != 0 && bloco != endNulo()) {
                disco[endInodeInd].inodeIndireto.endInd[inicio] = bloco;
                disco[endInodeInd].inodeIndireto.TL++;
                (*qtBlocos)--;
            }
            inicio++;
        }
    }
    if (inseridoT && *qtBlocos > 0) {
        disco[endInodeInd].inodeIndireto.endInd[disco[endInodeInd].inodeIndireto.TL] = criarInode(
            disco, usuario, tipoArq, *qtBlocos, endInode, "");
        if (disco[endInodeInd].inodeIndireto.endInd[disco[endInodeInd].inodeIndireto.TL] != endNulo())
            disco[endInodeInd].inodeIndireto.TL++;
    }
}

void inserirInodeID(Bloco *disco, int endInode, int endInodeInd, int *qtBlocos, int inseridoT, char *usuario,
                    char tipoArq) {
    if (disco[endInodeInd].inodeIndireto.TL < QTDE_INODE_INDIRETO) {
        int inicio = disco[endInodeInd].inodeIndireto.TL;
        while (inicio < *qtBlocos && inicio < QTDE_INODE_INDIRETO) {
            if (disco[endInodeInd].inodeIndireto.endInd[inicio] == endNulo()) {
                disco[endInodeInd].inodeIndireto.endInd[inicio] = criarInodeInd(disco);
                inserirInodeIS(disco, endInode, disco[endInodeInd].inodeIndireto.endInd[inicio], &*qtBlocos, inseridoT,
                               usuario, tipoArq);
                disco[endInodeInd].inodeIndireto.TL++;
            }
            inicio++;
        }
    }
}

void inserirInodeIT(Bloco *disco, int endInode, int endInodeInd, int *qtBlocos, char *usuario, char tipoArq) {
    if (disco[endInodeInd].inodeIndireto.TL < QTDE_INODE_INDIRETO) {
        int inicio = disco[endInodeInd].inodeIndireto.TL;
        while (inicio < *qtBlocos && inicio < QTDE_INODE_INDIRETO) {
            if (disco[endInodeInd].inodeIndireto.endInd[inicio] == endNulo()) {
                disco[endInodeInd].inodeIndireto.endInd[inicio] = criarInodeInd(disco);
                inserirInodeID(disco, endInode, disco[endInodeInd].inodeIndireto.endInd[inicio], &*qtBlocos, 0,
                               usuario, tipoArq);
                disco[endInodeInd].inodeIndireto.TL++;
            }
            inicio++;
        }
    }
}

int criarInode(Bloco *disco, char *usuario, char tipoArq, int tamanho, int endPai, char *caminho) {
    int endBloco, blocosNec, blocosRest, utilizados = 0, i;
    char permissao[11], data[30];

    blocosNec = (int) (ceil)((float) tamanho / (float) 10);
    if (blocosNec < qtdeBlocosLivres(disco)) {
        endBloco = popBlocoLivre(disco);
        for (i = 0; i < QTDE_INODE_DIRETO; i++)
            disco[endBloco].inode.endDireto[i] = endNulo();
        disco[endBloco].inode.endSimplesIndireto = endNulo();
        disco[endBloco].inode.endDuploIndireto = endNulo();
        disco[endBloco].inode.endTriploIndireto = endNulo();
        disco[endBloco].inode.contadorLink = 1;
        disco[endBloco].inode.tamanho = tamanho;
        switch (tipoArq) {
            case 'd':
                strcpy(permissao, "d");
                strcpy(permissao,PERMISSAO_DIRETORIO);
                break;
            case 'a':
                strcpy(permissao, "a");
                strcpy(permissao,PERMISSAO_ARQUIVO);
                break;
            case 'l':
                strcpy(permissao, "l");
                strcpy(permissao,PERMISSAO_LINK);
        }
        strcpy(disco[endBloco].inode.proprietario, usuario);
        strcpy(disco[endBloco].inode.grupo, usuario);
        strcpy(disco[endBloco].inode.permissao, permissao);
        horarioLocal(data);
        strcpy(disco[endBloco].inode.criacao, data);
        strcpy(disco[endBloco].inode.ultimaAlteracao, data);
        strcpy(disco[endBloco].inode.ultimoAcesso, data);

        i = 0;
        while (i < QTDE_INODE_DIRETO && i < blocosNec) {
            if (disco[endBloco].inode.endDireto[utilizados] == endNulo())
                disco[endBloco].inode.endDireto[utilizados++] = popBlocoLivre(disco);
            i++;
        }
        blocosRest = blocosNec - utilizados;
        if (blocosRest > 0) {
            if (disco[endBloco].inode.endSimplesIndireto == endNulo())
                disco[endBloco].inode.endSimplesIndireto = criarInodeInd(disco);
            inserirInodeIS(disco, endBloco, disco[endBloco].inode.endSimplesIndireto, &blocosRest, 0, usuario,
                           tipoArq);
        }
        if (blocosRest > 0) {
            if (disco[endBloco].inode.endDuploIndireto == endNulo())
                disco[endBloco].inode.endDuploIndireto = criarInodeInd(disco);
            inserirInodeID(disco, endBloco, disco[endBloco].inode.endDuploIndireto, &blocosRest, 0, usuario,
                           tipoArq);
        }
        if (blocosRest > 0) {
            if (disco[endBloco].inode.endTriploIndireto == endNulo())
                disco[endBloco].inode.endTriploIndireto = criarInodeInd(disco);
            inserirInodeIT(disco, endBloco, disco[endBloco].inode.endTriploIndireto, &blocosRest, usuario, tipoArq);
        }
        if (blocosRest > 0) {
            pushBlocoLivre(disco,
                           disco[disco[disco[disco[endBloco].inode.endTriploIndireto].inodeIndireto.endInd[4]].
                               inodeIndireto.
                               endInd[4]].inodeIndireto.endInd[4]);
            disco[disco[disco[disco[endBloco].inode.endTriploIndireto].inodeIndireto.endInd[4]].inodeIndireto.endInd
                        [4]]
                    .inodeIndireto.endInd[4] = criarInode(disco, usuario, tipoArq, blocosRest * 10 + 1, endPai,
                                                          caminho);
        }
        if (endPai == endNulo())
            endPai = endBloco;
        if (tipoArq == 'd') {
            adicionarArquivo(disco, disco[endBloco].inode.endDireto[0], ".", endBloco);
            adicionarArquivo(disco, disco[endBloco].inode.endDireto[0], "..", endPai);
        } else if (tipoArq == 'l')
            strcpy(disco[disco[endBloco].inode.endDireto[0]].softLink.caminho, caminho);
        return endBloco;
    }
    return endNulo();
}

void adicionarEntrada(Bloco *disco, int end, char *usuario, char *nomeEntrada, char tipo, int tam,
                      char *caminhoLink) {
    int i, j, k, endPai, indSimples, indDuplo, indTriplo;
    char achou;

    i = 0;
    while (i < QTDE_INODE_DIRETO && dirCheio(disco[disco[end].inode.endDireto[i]]))
        i++;
    if (i < QTDE_INODE_DIRETO && !dirCheio(disco[disco[end].inode.endDireto[i]])) {
        endPai = disco[disco[end].inode.endDireto[i]].dir.arquivo[1].endInode;
        if (disco[end].inode.endDireto[i] == endNulo()) {
            disco[end].inode.endDireto[i] = popBlocoLivre(disco);
            if (disco[end].inode.endDireto[i] != endNulo()) {
                adicionarArquivo(disco, disco[end].inode.endDireto[i], ".", end);
                adicionarArquivo(disco, disco[end].inode.endDireto[i], "..", endPai);
                adicionarArquivo(disco, disco[end].inode.endDireto[i], nomeEntrada,
                                 criarInode(disco, usuario, tipo, tam, end, caminhoLink));
            } else
                printf("Erro: Espaco em disco insuficiente!\n");
        } else if (!dirCheio(disco[disco[end].inode.endDireto[i]])) {
            adicionarArquivo(disco, disco[end].inode.endDireto[i], nomeEntrada,
                             criarInode(disco, usuario, tipo, tam, end, caminhoLink));
        }
    } else {
        endPai = disco[disco[end].inode.endDireto[0]].dir.arquivo[1].endInode;

        if (disco[end].inode.endSimplesIndireto == endNulo())
            disco[end].inode.endSimplesIndireto = criarInodeInd(disco);

        if (disco[end].inode.endSimplesIndireto == endNulo())
            printf("Erro: Espaco em disco insuficiente!\n");
        else {
            i = 0;
            indSimples = disco[end].inode.endSimplesIndireto;
            while (i < QTDE_INODE_INDIRETO && dirCheio(disco[disco[indSimples].inodeIndireto.endInd[i]]))
                i++;
            if (i < QTDE_INODE_INDIRETO && !dirCheio(disco[disco[indSimples].inodeIndireto.endInd[i]])) {
                if (disco[indSimples].inodeIndireto.endInd[i] == endNulo()) {
                    disco[indSimples].inodeIndireto.endInd[i] = popBlocoLivre(disco);
                    if (disco[indSimples].inodeIndireto.endInd[i] != endNulo()) {
                        adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[i], ".",
                                         end);
                        adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[i],
                                         "..", endPai);
                        adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[i],
                                         nomeEntrada, criarInode(disco, usuario, tipo, tam, end, caminhoLink));
                    } else
                        printf("Erro: Espaco em disco insuficiente!\n");
                } else if (!dirCheio(disco[disco[indSimples].inodeIndireto.endInd[i]])) {
                    adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[i],
                                     nomeEntrada, criarInode(disco, usuario, tipo, tam, end, caminhoLink));
                }
            } else {
                if (disco[end].inode.endDuploIndireto == endNulo())
                    disco[end].inode.endDuploIndireto = criarInodeInd(disco);
                if (disco[end].inode.endDuploIndireto == endNulo())
                    printf("Erro: Espaco em disco insuficiente!\n");
                else {
                    i = j = 0;
                    indDuplo = disco[end].inode.endDuploIndireto;
                    achou = 0;
                    while (i < QTDE_INODE_INDIRETO && !achou) {
                        j = 0;
                        if (disco[indDuplo].inodeIndireto.endInd[i] == endNulo())
                            disco[indDuplo].inodeIndireto.endInd[i] = criarInodeInd(disco);
                        indSimples = disco[indDuplo].inodeIndireto.endInd[i];
                        while (j < QTDE_INODE_INDIRETO &&
                               dirCheio(disco[disco[indSimples].inodeIndireto.endInd[j]]))
                            j++;
                        if (j < QTDE_INODE_INDIRETO)
                            achou = 1;
                        else
                            i++;
                    }
                    if (i < QTDE_INODE_INDIRETO && j < QTDE_INODE_INDIRETO) {
                        if (disco[indSimples].inodeIndireto.endInd[j] == endNulo()) {
                            disco[indSimples].inodeIndireto.endInd[j] = popBlocoLivre(disco);
                            if (disco[indSimples].inodeIndireto.endInd[j] != endNulo()) {
                                adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[j], ".",
                                                 end);
                                adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[j],
                                                 "..", endPai);
                                adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[j],
                                                 nomeEntrada,
                                                 criarInode(disco, usuario, tipo, tam, end, caminhoLink));
                            } else
                                printf("Erro: Espaco em disco insuficiente!\n");
                        } else if (!dirCheio(disco[disco[indSimples].inodeIndireto.endInd[j]]))
                            adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[j],
                                             nomeEntrada,
                                             criarInode(disco, usuario, tipo, tam, end, caminhoLink));
                    } else {
                        if (disco[end].inode.endTriploIndireto == endNulo())
                            disco[end].inode.endTriploIndireto = criarInodeInd(disco);
                        if (disco[end].inode.endTriploIndireto == endNulo())
                            printf("Erro: Espaco em disco insuficiente!\n");
                        else {
                            i = j = k = 0;
                            indTriplo = disco[end].inode.endTriploIndireto;
                            achou = 0;
                            while (i < QTDE_INODE_INDIRETO && !achou) {
                                j = 0;
                                if (disco[indTriplo].inodeIndireto.endInd[i] == endNulo())
                                    disco[indTriplo].inodeIndireto.endInd[i] = criarInodeInd(disco);
                                indDuplo = disco[indTriplo].inodeIndireto.endInd[i];
                                while (j < QTDE_INODE_INDIRETO && !achou) {
                                    k = 0;
                                    if (disco[indDuplo].inodeIndireto.endInd[j] == endNulo())
                                        disco[indDuplo].inodeIndireto.endInd[j] = criarInodeInd(disco);
                                    indSimples = disco[indDuplo].inodeIndireto.endInd[j];
                                    while (k < QTDE_INODE_INDIRETO && dirCheio(
                                               disco[disco[indSimples].inodeIndireto.endInd[k]]))
                                        k++;
                                    if (k < QTDE_INODE_INDIRETO)
                                        achou = 1;
                                    else
                                        j++;
                                }
                                if (j < QTDE_INODE_INDIRETO)
                                    achou = 1;
                                else
                                    i++;
                            }
                            if (i < QTDE_INODE_INDIRETO && j < QTDE_INODE_INDIRETO && k < QTDE_INODE_INDIRETO) {
                                if (disco[indSimples].inodeIndireto.endInd[k] == endNulo()) {
                                    disco[indSimples].inodeIndireto.endInd[k] = popBlocoLivre(disco);
                                    if (disco[indSimples].inodeIndireto.endInd[k] != endNulo()) {
                                        adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[k], ".",
                                                         end);
                                        adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[k],
                                                         "..", endPai);
                                        adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[k],
                                                         nomeEntrada,
                                                         criarInode(disco, usuario, tipo, tam, end, caminhoLink));
                                    } else
                                        printf("Erro: Espaco em disco insuficiente!\n");
                                } else if (!dirCheio(disco[disco[indSimples].inodeIndireto.endInd[k]])) {
                                    adicionarArquivo(disco, disco[indSimples].inodeIndireto.endInd[k],
                                                     nomeEntrada,
                                                     criarInode(disco, usuario, tipo, tam, end, caminhoLink));
                                }
                            } else
                                adicionarEntrada(disco, disco[indSimples].inodeIndireto.endInd[k], usuario,
                                                 nomeEntrada, tipo, tam, caminhoLink);
                        }
                    }
                }
            }
        }
    }
}

int adicionarEntradasRaiz(Bloco *disco, int endRaiz, char *usuario) {
    int endUsuario;

    adicionarEntrada(disco, endRaiz, usuario, "home", 'd', 1, "");
    adicionarEntrada(disco, disco[disco[endRaiz].inode.endDireto[0]].dir.arquivo[2].endInode, usuario,
                     usuario, 'd', 1, "");
    endUsuario = disco[disco[disco[disco[endRaiz].inode.endDireto[0]].dir.arquivo[2].endInode].inode.
                endDireto[0]].dir.
            arquivo[2].endInode;
    adicionarEntrada(disco, endUsuario, usuario, "Documentos", 'd', 1, "");
    adicionarEntrada(disco, endUsuario, usuario, "Downloads", 'd', 1, "");
    adicionarEntrada(disco, endUsuario, usuario, "Imagens", 'd', 1, "");
    adicionarEntrada(disco, endUsuario, usuario, "Musicas", 'd', 1, "");
    adicionarEntrada(disco, endUsuario, usuario, "Publico", 'd', 1, "");
    adicionarEntrada(disco, endUsuario, usuario, "Videos", 'd', 1, "");
    adicionarEntrada(disco, endRaiz, usuario, "bin", 'd', 1, "");
    adicionarEntrada(disco, endRaiz, usuario, "boot", 'd', 1, "");
    adicionarEntrada(disco, endRaiz, usuario, "dev", 'd', 1, "");
    adicionarEntrada(disco, endRaiz, usuario, "etc", 'd', 1, "");
    adicionarEntrada(disco, endRaiz, usuario, "lib", 'd', 1, "");
    adicionarEntrada(disco, endRaiz, usuario, "opt", 'd', 1, "");

    return endUsuario;
}
