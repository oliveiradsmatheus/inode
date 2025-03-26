#define MAXPILHA 10

struct pilha {
    int TOPO[MAXPILHA], BASE[MAXPILHA];
    char PILHA[MAXPILHA];
};
typedef struct pilha Pilha;

void inicializar(Pilha *PM, int qtde) {
    int qtdeElem, i;

    qtdeElem = MAXPILHA / qtde;
    for (i = 0; i <= qtde; i++) {
        (*PM).BASE[i] = (i * qtdeElem);
        (*PM).TOPO[i] = (*PM).BASE[i] - 1;
    }
}

void push(Pilha *PM, char elem, int NP) {
    (*PM).PILHA[++(*PM).TOPO[NP]] = elem;
}

char pop(Pilha *PM, int NP) {
    return (*PM).PILHA[(*PM).TOPO[NP]--];
}

char elementoTopo(Pilha PM, int NP) {
    return PM.PILHA[PM.TOPO[NP]];
}

int listaVazia(Pilha PM, int NP) {
    return (PM.TOPO[NP] + 1 == PM.BASE[NP]);
}

int listaCheia(Pilha PM, int NP) {
    return (PM.BASE[NP + 1] == PM.TOPO[NP] + 1);
}

void exibir(Pilha PM, int NP) {
    while (!listaVazia(PM, NP))
        printf("\n[%c]", pop(&PM, NP));
}