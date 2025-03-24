struct inode {
};
typedef struct inode iNode;

struct diretorio {

};
typedef struct diretorio Dir;

struct bloco {
    int bad;
    iNode inode;
    Dir dir;
};
typedef struct bloco Bloco;