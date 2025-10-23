# 🧠 Simulador de Sistema de Arquivos (Inodes) Linux

💻 **Disciplina:** Sistemas Operacionais I  
📚 **Curso:** Ciência da Computação — FIPP / UNOESTE  
👨‍💻 **Autor:** Matheus Oliveira da Silva  

---

## 📖 Sobre o Projeto

Este projeto implementa um **simulador de sistema de arquivos** inspirado no modelo de **inodes do Linux**, com interface em **modo texto (console)**.

O objetivo é **demonstrar o funcionamento interno do sistema operacional** no gerenciamento de dados em disco — desde a alocação de blocos até a estrutura de diretórios — abordando **inodes, blocos de dados, links simbólicos e físicos, e gerenciamento de espaço livre.**

---

## 🎯 Objetivos

- Compreender como o **sistema de arquivos Linux** gerencia arquivos usando **inodes**.
- Simular a **alocação de blocos** (direta, indireta simples, dupla e tripla).
- Implementar a **estrutura de diretórios** hierárquica (incluindo arquivos `.` e `..`).
- Gerenciar uma **lista de blocos livres** (implementada como uma pilha encadeada).
- Simular comandos de shell padrão: `ls`, `mkdir`, `rm`, `cd`, `ln`, `touch`, `chmod`, `tree`, `pwd`, etc.
- Implementar **links físicos (hard links)** e **links simbólicos (soft links)**.
- Proporcionar uma **interface de terminal (shell)** amigável em modo texto, com cores para diferenciar tipos de arquivos.

---

## 🚀 Compilando e Executando

Este projeto foi desenvolvido em C e pode ser compilado no Linux com o `gcc`.

1.  Certifique-se de ter o `gcc` instalado.
2.  No terminal, navegue até a pasta do projeto.
3.  Execute o comando de compilação (assumindo que seu arquivo principal se chama `main.c`):

    ```bash
    gcc main.c -o simulador -lm
    ```

    * **`main.c`**: É o seu arquivo-fonte `.c` que contém a função `main()`.
    * **`-o simulador`**: Define o nome do arquivo executável de saída (você pode mudar "simulador" para o que quiser).
    * **`-lm`**: É **essencial** para linkar a biblioteca matemática, usada para a função `ceil()` no projeto.

4.  Para executar o programa:

    ```bash
    ./simulador
    ```
---

## ⚙️ Funcionalidades Principais

- 🔹 **Estrutura de Inode:** Implementação de inodes com permissões, timestamps, proprietário, grupo, contagem de links e ponteiros de blocos.
- 🔹 **Alocação de Blocos:** Suporte a endereçamento direto, indireto simples, duplo e triplo para arquivos de tamanhos variados.
- 🔹 **Gerenciamento de Disco:** Simulação de um "disco" como um array de blocos e gerenciamento de uma lista de blocos livres.
- 🔹 **Shell Interativo:** Um terminal que aceita e executa comandos como `ls (-l)`, `cd`, `mkdir`, `rm`, `rmdir`, `pwd`, `tree`, `df`, e `clear`.
- 🔹 **Links:** Criação e remoção de links físicos (`link -h`) e simbólicos (`link -s`).
- 🔹 **Simulação de "Bad Blocks":** Capacidade de marcar blocos como defeituosos (`bad`) e verificar a integridade do sistema (`status`, `perdidos`).
- 🔹 **Gerenciamento de Arquivos:** Criação (`touch`), remoção (`rm`), alteração de permissões (`chmod`) e visualização (`vi` simulado).
- 🔹 **Interface Colorida:** Uso de códigos ANSI (`cores.h`) para diferenciar arquivos (branco), diretórios (azul) e links (ciano).

---

## 🧩 Estrutura de Dados Utilizadas

O simulador é construído sobre estruturas que mimetizam os componentes de um sistema de arquivos real:

| Estrutura | Tipo | Descrição |
| :--- | :--- | :--- |
| `Bloco` | Struct | A unidade fundamental do disco. Pode atuar como Inode, Bloco de Diretório, Bloco Indireto, etc. |
| `Inode` | Struct | Contém os metadados do arquivo (permissões, dono, tamanho, timestamps, ponteiros de blocos). |
| `Diretorio` | Struct | Armazena um conjunto de `Entrada` (arquivos/diretórios filhos). |
| `Entrada` | Struct | Uma entrada de diretório (par nome-inode). |
| `InodeInd` | Struct | Bloco de endereçamento indireto (usado para indireção simples, dupla e tripla). |
| `ListaBlocosLivres`| Struct | Estrutura para gerenciar blocos livres (implementada como uma pilha). |
| `SoftLink` | Struct | Armazena o caminho de destino de um link simbólico. |

---

## 💾 Arquitetura do Sistema de Arquivos

O funcionamento do simulador segue a lógica do modelo de inodes:

- O disco é simulado como um grande vetor de `Bloco`.
- Um conjunto inicial de blocos é reservado para a **Lista de Blocos Livres**.
- O **Inode Raiz ( / )** é criado no primeiro bloco livre disponível.
- O sistema de arquivos é inicializado com uma estrutura de diretórios padrão ( `/home`, `/bin`, `/etc`, e o diretório do usuário `/home/<usuario>`).

**Ao criar um arquivo (`touch` ou `mkdir`):**
1.  Um bloco livre é alocado da pilha para o **Inode** do novo arquivo.
2.  Outros blocos livres são alocados para os **blocos de dados** (ou blocos de diretório).
3.  Os ponteiros no Inode (direto, indireto) são atualizados para apontar para esses blocos de dados.
4.  Uma `Entrada` (nome, nº do inode) é adicionada ao `Diretorio` pai.

**Ao remover um arquivo (`rm` ou `rmdir`):**
1.  A `Entrada` é removida do `Diretorio` pai.
2.  Para links físicos, o `contadorLink` do inode é decrementado.
3.  Se `contadorLink` chegar a zero (ou se for um diretório/link simbólico), seus blocos de dados e seu próprio Inode são devolvidos à **Lista de Blocos Livres**.

---

## 🧠 Pseudocódigo da Resolução de Caminho (Busca)

A função `buscaArquivo` é central para o sistema, localizando um inode a partir de um diretório pai e um nome.

```c
função buscaArquivo(disco, endDir, nomeArquivo, &posicao, &endereco):
    // 1. Verificar Blocos Diretos
    para i de 0 até QTDE_INODE_DIRETO:
        blocoDir = disco[endDir].inode.endDireto[i]
        para j de 0 até disco[blocoDir].dir.TL:
            se disco[blocoDir].dir.arquivo[j].nome == nomeArquivo:
                posicao = j
                endereco = blocoDir
                retornar SUCESSO

    // 2. Verificar Indireto Simples
    se disco[endDir].inode.endSimplesIndireto != NULO:
        blocoIndSimples = disco[endDir].inode.endSimplesIndireto
        para i de 0 até QTDE_INODE_INDIRETO:
            blocoDir = disco[blocoIndSimples].inodeIndireto.endInd[i]
            para j de 0 até disco[blocoDir].dir.TL:
                se disco[blocoDir].dir.arquivo[j].nome == nomeArquivo:
                    posicao = j
                    endereco = blocoDir
                    retornar SUCESSO

    // 3. Verificar Indireto Duplo
    se disco[endDir].inode.endDuploIndireto != NULO:
        blocoIndDuplo = disco[endDir].inode.endDuploIndireto
        para i de 0 até QTDE_INODE_INDIRETO:
            blocoIndSimples = disco[blocoIndDuplo].inodeIndireto.endInd[i]
            para j de 0 até QTDE_INODE_INDIRETO:
                blocoDir = disco[blocoIndSimples].inodeIndireto.endInd[j]
                // ... busca as entradas no blocoDir ...
                se achou:
                    retornar SUCESSO

    // 4. Verificar Indireto Triplo
    // ... lógica similar ...

    retornar FALHA
```
