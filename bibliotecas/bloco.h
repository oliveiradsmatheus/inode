#define CABECA_LISTA_BL 0;
#define MAXIMO_BL 9;
#include <math.h>

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
    char criacao[12];
    char ultimoAcesso[12];
    char ultimaAlteracao[12];
    int tamanho;
    int endDireto[5];
    int endSimplesIndireto;
    int endDuploIndireto;
    int endTriploIndireto;
};

typedef struct inode Inode;

struct softlink {
    char caminho[50];
};

typedef struct softlink SoftLink;

struct entrada {
    char nome[14];
    int endInode;
};

typedef struct entrada Entrada;

struct diretorio {
    Entrada arquivo[10];
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
    int bad;
    Inode inode;
    Diretorio dir;
    ListaBlocosLivres listaBlocosLivres;
    SoftLink softLink;
};

typedef struct bloco Bloco;

void inicializarBloco(Bloco *bloco) {
    (*bloco).bad = 0;
    (*bloco).dir.TL = 0;
    (*bloco).inode.permissao[0] = '\0';
    (*bloco).listaBlocosLivres.topo = -1;
    (*bloco).softLink.caminho[0] = '\0';
}

void inicializarDisco(Bloco *disco, int tamDisco) {
    int i;
    for (i = 0; i < tamDisco; i++)
        inicializarBloco(&disco[i]);
}

int criarInode(Bloco *disco, char tipoArquivo, char permissao[10], int tamanho, int endPai, char link) {
}

void ligarListaBlocosLivres(Bloco *bloco, int qtde) {
    int i;
    for (i = 0; i < qtde; i++)
        bloco[i].listaBlocosLivres.endProxLista = i + 1;
    bloco[i - 1].listaBlocosLivres.endProxLista = -1;
}

int vazia(Bloco disco) {
    return disco.listaBlocosLivres.topo == -1;
}

int cheia(Bloco disco) {
    return disco.listaBlocosLivres.topo == MAXIMO_BL;
}

int fim(Bloco disco) {
    return disco.listaBlocosLivres.endProxLista == -1;
}

int popBlocoLivre(Bloco *disco) {
    int end, endProx;

    end = CABECA_LISTA_BL;
    endProx = disco[end].listaBlocosLivres.endProxLista;
    while (vazia(disco[end])) {
        end = endProx;
        endProx = disco[end].listaBlocosLivres.endProxLista;
    }
    if (!vazia(disco[end]))
        return disco[end].listaBlocosLivres.end[disco[end].listaBlocosLivres.topo--];
}

void pushBlocoLivre(Bloco *disco, int bloco) {
    int end, endProx;

    end = CABECA_LISTA_BL;
    endProx = disco[end].listaBlocosLivres.endProxLista;
    while (vazia(disco[end]) && !fim(disco[end])) {
        end = endProx;
        endProx = disco[end].listaBlocosLivres.endProxLista;
    }
    if (cheia(disco[end]))
        end -= 1;
    disco[end].listaBlocosLivres.end[++disco[end].listaBlocosLivres.topo] = bloco;
}

void exibirPilhas(Bloco *disco) {
    // Apenas para teste.
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
    int i, qtde;

    i = CABECA_LISTA_BL;
    qtde = ceil(((float) tamDisco) / 11);
    ligarListaBlocosLivres(disco, qtde);
    exibirPilhas(disco);
    i = tamDisco - 1;
    while (i >= qtde)
        pushBlocoLivre(disco, i--);
}

void iniciarBlocos(char *usuario, int *blocos) {
    //criar root, dev, bin, home... respectivos inodes e pilhas de blocos livres.
}
