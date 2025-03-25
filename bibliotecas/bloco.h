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
    int end;
    int endProx;
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

void inicializarBloco(Bloco bloco) {
    bloco.bad = 0;
    bloco.dir.TL = 0;
    bloco.inode.permissao[0] = '\0';
    bloco.listaBlocosLivres.topo = -1;
    bloco.softLink.caminho[0] = '\0';
}

void inicializarDisco(Bloco *disco, int tamDisco) {
    int i;
    for (i = 0; i < tamDisco; i++)
        inicializarBloco(disco[i]);
}

int criarInode(Bloco *disco, char tipoArquivo, char permissao[10], int tamanho, int endPai, char link) {
    
}