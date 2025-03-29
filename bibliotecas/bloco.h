#define CABECA_LISTA_BL 0
#define MAXIMO_BL 9
#define PERMISSAO_ARQUIVO "rw-r--r--"
#define PERMISSAO_DIRETORIO "rwxr-xr-x"
#define PERMISSAO_LINK "rwxrwxrwx"
#define QTDE_MAX_DIR 12
#define TAM_MAX_NOME 15
#define QTDE_INODE_DIRETO 5
#define QTDE_INODE_INDIRETO 5

struct inode {
    /*
        DEFINÇÃO DO QUE O I-NODE ESTÁ APONTANDO:
        [0] 'd' = DIRETÓRIO, '-' = ARQUIVO, 'l' = LINK

        PERMISSÕES:
        [1] - [3] (rwx) -> OWNER
        [4] - [6] (rwx) -> GROUP
        [7] - [9] (rwx) -> OTHERS

        [10] - '\0'
    */
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
    for (int i = 0; i < 3; i++) {
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

    for (int i = 0; i < 3; i++) {
        data[i] = dia[i];
        data[i + 4] = mes[i];
    }
}

int endNulo(void) {
    return -1;
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

char listaVazia(Bloco disco) {
    return disco.listaBlocosLivres.topo == endNulo();
}

char listaCheia(Bloco disco) {
    return disco.listaBlocosLivres.topo == MAXIMO_BL;
}

char fimLista(Bloco disco) {
    return disco.listaBlocosLivres.endProxLista == endNulo();
}

int qtdeBlocosLivres(Bloco *disco) {
    int qtde = 0, end = CABECA_LISTA_BL;

    while (!fimLista(disco[end])) {
        qtde += disco[end].listaBlocosLivres.topo + 1;
        end = disco[end].listaBlocosLivres.endProxLista;
    }
    qtde += disco[end].listaBlocosLivres.topo + 1;

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
    if (!listaVazia(disco[end])) {
        if (bad(disco[end]))
            return popBlocoLivre(disco);
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
            printf("[   %d   ] \n", disco[i].listaBlocosLivres.end[j]);
        printf("End. prox: %d\n", disco[i].listaBlocosLivres.endProxLista);
        i = disco[i].listaBlocosLivres.endProxLista;
    }
    printf("Pilha %d\nTopo: %d\n", i, disco[i].listaBlocosLivres.topo);
    for (j = 0; j < 10; j++)
        printf("[   %d   ] \n", disco[i].listaBlocosLivres.end[j]);
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
    return disco.dir.TL == 0;
}

char dirCheio(Bloco disco) {
    return disco.dir.TL == QTDE_MAX_DIR;
}

void adicionarArquivo(Bloco *disco, int endDir, char *nomeArquivo, int endInode) {
    disco[endDir].dir.arquivo[disco[endDir].dir.TL].endInode = endInode;
    strcpy(disco[endDir].dir.arquivo[disco[endDir].dir.TL++].nome, nomeArquivo);
}

void iniciarBlocos(char *usuario, int *blocos) {
    //criar root, dev, bin, home... respectivos inodes e pilhas de blocos livres.
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
    for (i = 0; i < QTDE_INODE_INDIRETO; i++)
        disco[bloco].inodeIndireto.endInd[i] = -1;
    disco[bloco].inodeIndireto.TL = 0;

    return bloco;
}

void inserirInodeIS(Bloco *disco, int endInode, int endInodeInd, int *qtBlocos, int inseridoT, char *usuario,
                    char tipoArq) {
    int utilizados = 0;


    if (disco[endInodeInd].inodeIndireto.TL < QTDE_INODE_INDIRETO - inseridoT) {
        int inicio = disco[endInodeInd].inodeIndireto.TL;
        while (inicio < *qtBlocos && inicio < QTDE_INODE_INDIRETO - inseridoT) {
            disco[endInodeInd].inodeIndireto.endInd[inicio] = popBlocoLivre(disco);
            disco[endInodeInd].inodeIndireto.TL++;
            utilizados++;
            inicio++;
        }
    }
    *qtBlocos = *qtBlocos - utilizados;
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
            if (disco[endInodeInd].inodeIndireto.endInd[inicio] == endNulo())
                disco[endInodeInd].inodeIndireto.endInd[inicio] = criarInodeInd(disco);
            inserirInodeIS(disco, endInode, disco[endInodeInd].inodeIndireto.endInd[inicio], &*qtBlocos, inseridoT,
                           usuario, tipoArq);
            disco[endInodeInd].inodeIndireto.TL++;
            inicio++;
        }
    }
}

void inserirInodeIT(Bloco *disco, int endInode, int endInodeInd, int *qtBlocos, char *usuario, char tipoArq) {
    if (disco[endInodeInd].inodeIndireto.TL < QTDE_INODE_INDIRETO) {
        int inicio = disco[endInodeInd].inodeIndireto.TL;
        while (inicio < *qtBlocos && inicio < QTDE_INODE_INDIRETO) {
            if (disco[endInodeInd].inodeIndireto.endInd[inicio] == endNulo())
                disco[endInodeInd].inodeIndireto.endInd[inicio] = criarInodeInd(disco);
            inserirInodeID(disco, endInode, disco[endInodeInd].inodeIndireto.endInd[inicio], &*qtBlocos, 1,
                           usuario, tipoArq);
            disco[endInodeInd].inodeIndireto.TL++;
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
                strcat(permissao,PERMISSAO_DIRETORIO);
                break;
            case 'a':
                strcpy(permissao, "-");
                strcat(permissao,PERMISSAO_ARQUIVO);
                break;
            case 'l':
                strcpy(permissao, "l");
                strcat(permissao,PERMISSAO_LINK);
        }
        strcpy(disco[endBloco].inode.proprietario, usuario);
        strcpy(disco[endBloco].inode.grupo, usuario);
        strcpy(disco[endBloco].inode.permissao, permissao);
        horarioLocal(data);
        strcpy(disco[endBloco].inode.criacao, data);
        strcpy(disco[endBloco].inode.ultimaAlteracao, data);
        strcpy(disco[endBloco].inode.ultimoAcesso, data);

        i = 0;
        while (i < blocosNec && i < 5) {
            if (disco[endBloco].inode.endDireto[utilizados] == endNulo())
                disco[endBloco].inode.endDireto[utilizados++] = popBlocoLivre(disco);
            i++;
        }
        blocosRest = blocosNec - utilizados;
        if (blocosRest > 0) {
            if (disco[endBloco].inode.endSimplesIndireto == endNulo())
                disco[endBloco].inode.endSimplesIndireto = criarInodeInd(disco);
            inserirInodeIS(disco, endBloco, disco[endBloco].inode.endSimplesIndireto, &blocosRest, 0, usuario, tipoArq);
        }
        if (blocosRest > 0) {
            if (disco[endBloco].inode.endDuploIndireto == endNulo())
                disco[endBloco].inode.endDuploIndireto = criarInodeInd(disco);
            inserirInodeID(disco, endBloco, disco[endBloco].inode.endDuploIndireto, &blocosRest, 0, usuario, tipoArq);
        }
        if (blocosRest > 0) {
            if (disco[endBloco].inode.endTriploIndireto == endNulo())
                disco[endBloco].inode.endTriploIndireto = criarInodeInd(disco);
            inserirInodeIT(disco, endBloco, disco[endBloco].inode.endTriploIndireto, &blocosRest, usuario, tipoArq);
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

void adicionarEntrada(Bloco *disco, int end, char *usuario, char *nomeEntrada, char tipo, int tam) {
    int i;

    i = 0;
    while (dirCheio(disco[disco[end].inode.endDireto[i]]))
        i++;
    if (i < QTDE_INODE_DIRETO) {
        if (disco[end].inode.endDireto[i] != -1) {
            if (disco[disco[end].inode.endDireto[i]].dir.TL < QTDE_MAX_DIR)
                adicionarArquivo(disco, disco[end].inode.endDireto[i], nomeEntrada,
                                 criarInode(disco, usuario, tipo, tam, end, ""));
        } else {
            disco[end].inode.endDireto[i] = popBlocoLivre(disco);
            adicionarArquivo(disco, disco[end].inode.endDireto[i], nomeEntrada,
                             criarInode(disco, usuario, tipo, tam, end, ""));
        }
    } else {

    }
}

void adicionarEntradasRaiz(Bloco *disco, int endRaiz, char *usuario) {
    adicionarEntrada(disco, endRaiz, usuario, "bin", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "boot", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "dev", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "etc", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "home", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "lib", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "opt", 'd', 1);
    /*adicionarEntrada(disco, endRaiz, usuario, "1", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "2", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "3", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "4", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "5", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "6", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "7", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "8", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "9", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "1", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "2", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "3", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "4", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "5", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "6", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "7", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "8", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "9", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "1", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "2", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "3", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "4", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "5", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "6", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "7", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "8", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "9", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "1", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "2", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "3", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "4", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "5", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "6", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "7", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "8", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "9", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "1", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "2", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "3", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "4", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "5", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "6", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "7", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "8", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "9", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "1", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "2", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "3", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "4", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "5", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "6", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "7", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "8", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "9", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "1", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "2", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "3", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "4", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "5", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "6", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "7", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "8", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "9", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "1", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "2", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "3", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "4", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "5", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "6", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "7", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "8", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "9", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "1", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "2", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "3", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "4", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "5", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "6", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "7", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "8", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "9", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "1", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "2", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "3", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "4", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "5", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "6", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "7", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "8", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "9", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "1", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "2", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "3", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "4", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "5", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "6", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "7", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "8", 'd', 1);
    adicionarEntrada(disco, endRaiz, usuario, "9", 'd', 1);*/
}
