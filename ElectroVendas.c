// CADASTRO DE LOJA DE ELETRONICOS


#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h>   

#define ARQUIVO    "loja.txt"   // nome do arquivo principal com os dados
#define TEMPORARIO "temp.txt"   // nome do arquivo temporario (usado em alterar/excluir)
#define CSV        "loja.csv"   // nome do arquivo de exportacao
#define MAX 200                 // maximo de produtos que cabem na memoria

// Cada produto e um "registro" com varios campos (struct)
typedef struct {                // inicio da definicao da estrutura
    int   id;                   // numero do produto (gerado sozinho)
    char  produto[50];          // nome do produto (aumente se precisar de mais espaco)
    char  categoria[30];        // categoria do produto
    float preco;                // preco em reais
    int   quantidade;           // quantidade em estoque
    char  fornecedor[40];       // nome do fornecedor
} Produto;                      // "Produto" passa a ser o nome desse tipo

/* ================= FUNCOES AUXILIARES =============== */

// Tira o ENTER (\n) do final do texto
void removerEnter(char *texto) {            // recebe o endereco do texto (ponteiro)
    char *p = strchr(texto, '\n');          // procura o \n e guarda o endereco dele em p
    if (p != NULL) {                        // se achou o \n...
        *p = '\0';                          // ...troca o \n por \0 (fim do texto)
    }                                       // fim do if
}                                           // fim da funcao

// Mostra uma pergunta na tela e le o texto digitado
void lerTexto(const char *pergunta, char *destino, int tamanho) {
    printf("%s", pergunta);                         // mostra a pergunta
    if (fgets(destino, tamanho, stdin) == NULL) {   // le uma linha; NULL = nao ha mais entrada
        destino[0] = '\0';                          // sem entrada: deixa o texto vazio
        printf("\n");                               // pula uma linha na tela
        return;                                     // sai da funcao
    }                                               // fim do if
    removerEnter(destino);                          // tira o ENTER do final do texto
    printf("%s\n", destino);                        // repete na tela o que foi digitado
}                                                   // fim da funcao

// Le um numero inteiro (le como texto e depois converte)
int lerInteiro(const char *pergunta) {
    char texto[50];                                 // guarda o que foi digitado
    lerTexto(pergunta, texto, sizeof(texto));       // le o texto do usuario
    return atoi(texto);                             // converte o texto em int e devolve
}

// Le um numero com virgula (aceita 49,90 ou 49.90)
float lerReal(const char *pergunta) {
    char texto[50];                                 // guarda o que foi digitado
    lerTexto(pergunta, texto, sizeof(texto));       // le o texto do usuario
    for (char *p = texto; *p != '\0'; p++) {        // percorre o texto com ponteiro, letra por letra
        if (*p == ',') {                            // se a letra for virgula...
            *p = '.';                               // ...troca por ponto
        }                                           // fim do if
    }                                               // fim do for
    return (float)atof(texto);                      // converte o texto em float e devolve
}

// Copia um texto para outro, tudo em letra minuscula
void paraMinusculo(const char *origem, char *destino) {
    while (*origem != '\0') {                               // repete ate o fim do texto de origem
        *destino = (char)tolower((unsigned char)*origem);   // converte 1 letra e guarda no destino
        origem++;                                           // vai para a proxima letra da origem
        destino++;                                          // vai para a proxima posicao do destino
    }                                                       // fim do while
    *destino = '\0';                                        // fecha o texto do destino
}

// Compara dois textos ignorando maiuscula/minuscula
// Retorna 0 se forem iguais, negativo se "a" vem antes de "b", positivo se vem depois
int compararTexto(const char *a, const char *b) {
    char minA[100], minB[100];          // copias dos textos em minusculo
    paraMinusculo(a, minA);             // coloca "a" em minusculo dentro de minA
    paraMinusculo(b, minB);             // coloca "b" em minusculo dentro de minB
    return strcmp(minA, minB);          // strcmp compara os dois textos e devolve o resultado
}

// Retorna 1 se "trecho" aparece dentro de "texto" (ignora maiuscula/minuscula)
int contemTexto(const char *texto, const char *trecho) {
    char minTexto[100], minTrecho[100];             // copias em minusculo
    paraMinusculo(texto, minTexto);                 // texto em minusculo
    paraMinusculo(trecho, minTrecho);               // trecho em minusculo
    return strstr(minTexto, minTrecho) != NULL;     // 1 se encontrou, 0 se nao encontrou
}

// Troca ; por , (o ; e o separador do arquivo, entao nao pode aparecer nos textos)
void limparSeparador(char *texto) {
    for (; *texto != '\0'; texto++) {       // percorre o texto ate o fim
        if (*texto == ';') {                // se a letra for ponto e virgula...
            *texto = ',';                   // ...troca por virgula
        }                                   // fim do if
    }                                       // fim do for
}

// Copia "novo" para "destino" somente se o usuario digitou alguma coisa
void copiarSeNaoVazio(char *destino, const char *novo, int tamanho) {
    if (strlen(novo) > 0) {                     // o usuario digitou algo?
        strncpy(destino, novo, tamanho - 1);    // copia sem passar do tamanho do campo
        destino[tamanho - 1] = '\0';            // garante que o texto termina certinho
    }                                           // fim do if
}

/* ============== LER E GRAVAR PRODUTOS NO ARQUIVO ============== */

// Transforma uma linha do arquivo em um Produto
// Retorna 1 se leu os 6 campos, 0 se a linha esta errada
// %49[^;] quer dizer: le ate 49 caracteres que NAO sejam ;
int linhaParaProduto(const char *linha, Produto *p) {
    int lidos = sscanf(linha, "%d;%49[^;];%29[^;];%f;%d;%39[^\n]",  // le os campos da linha
                       &p->id, p->produto, p->categoria,            // guarda id, nome e categoria
                       &p->preco, &p->quantidade, p->fornecedor);   // guarda preco, quantidade e fornecedor
    return lidos == 6;                                              // 1 se leu os 6 campos
}

// Escreve um produto (uma linha) no arquivo que ja esta aberto
void gravarProduto(FILE *arquivo, const Produto *p) {
    fprintf(arquivo, "%d;%s;%s;%.2f;%d;%s\n",      // escreve os campos separados por ;
            p->id, p->produto, p->categoria,        // id, nome e categoria
            p->preco, p->quantidade, p->fornecedor);// preco (2 casas), quantidade e fornecedor
}

// Le todos os produtos do arquivo para o vetor "lista"
// Retorna quantos produtos foram lidos
int carregarProdutos(Produto *lista) {
    FILE *arquivo = fopen(ARQUIVO, "r");        // abre o arquivo para leitura
    if (arquivo == NULL) {                      // se o arquivo nao existe...
        return 0;                               // ...devolve 0 produtos
    }                                           // fim do if
    char linha[250];                            // guarda cada linha lida
    int n = 0;                                  // contador de produtos lidos
    while (n < MAX && fgets(linha, sizeof(linha), arquivo) != NULL) {   // le linha por linha
        if (linhaParaProduto(linha, &lista[n])) {   // converte a linha em produto
            n++;                                    // so conta se a linha for valida
        }                                           // fim do if
    }                                               // fim do while
    fclose(arquivo);                            // fecha o arquivo
    return n;                                   // devolve quantos produtos foram lidos
}

// Gera o proximo ID: maior ID que existe + 1
int gerarId(void) {
    Produto lista[MAX];                                 // vetor temporario
    int n = carregarProdutos(lista);                    // carrega os produtos do arquivo
    int maior = 0;                                      // guarda o maior ID encontrado
    for (Produto *p = lista; p < lista + n; p++) {      // percorre o vetor usando ponteiro
        if (p->id > maior) {                            // este ID e maior que o guardado?
            maior = p->id;                              // entao guarda este
        }                                               // fim do if
    }                                                   // fim do for
    return maior + 1;                                   // proximo ID livre
}

// Retorna 1 se ja existe produto com esse nome
// "idIgnorar" serve para o ALTERAR nao reclamar do proprio produto (use 0 ao inserir)
int nomeDuplicado(const char *nome, int idIgnorar) {
    Produto lista[MAX];                                 // vetor temporario
    int n = carregarProdutos(lista);                    // carrega os produtos do arquivo
    for (Produto *p = lista; p < lista + n; p++) {      // percorre o vetor usando ponteiro
        if (p->id != idIgnorar && compararTexto(p->produto, nome) == 0) {   // outro produto com o mesmo nome?
            return 1;                                   // achou nome igual: e duplicado
        }                                               // fim do if
    }                                                   // fim do for
    return 0;                                           // nenhum igual: nome livre
}

/* ===================== MOSTRAR EM TABELA ===================== */

// Mostra os produtos de um vetor em formato de tabela (nao mexe no arquivo)
void mostrarTabela(const Produto *lista, int n) {
    printf("\n---------------------------------------------------------------------------------\n");     // linha de cima
    printf("| ID | Produto              | Categoria        | Preco (R$) | Qtd   | Fornecedor     |\n");   // cabecalho
    printf("---------------------------------------------------------------------------------\n");       // linha de baixo do cabecalho
    for (const Produto *p = lista; p < lista + n; p++) {            // percorre o vetor usando ponteiro
        printf("| %-2d | %-20s | %-16s | %10.2f | %-5d | %-14s |\n", // imprime uma linha da tabela
               p->id, p->produto, p->categoria,                     // id, nome e categoria
               p->preco, p->quantidade, p->fornecedor);             // preco, quantidade e fornecedor
    }                                                               // fim do for
    printf("---------------------------------------------------------------------------------\n");       // linha final
    printf("Total: %d produto(s)\n", n);                            // mostra quantos produtos foram exibidos
}

/* ======================= OPERACOES ======================= */

// CRIAR: cria o arquivo vazio (apaga o anterior)
void criarArquivo(int mostrarMensagem) {
    FILE *arquivo = fopen(ARQUIVO, "w");        // "w" cria o arquivo (ou apaga e cria de novo)
    if (arquivo == NULL) {                      // se deu erro ao criar...
        printf("Erro ao criar o arquivo!\n");   // ...avisa o usuario
        return;                                 // ...e sai da funcao
    }                                           // fim do if
    fclose(arquivo);                            // fecha o arquivo (ele fica vazio)
    if (mostrarMensagem) {                      // so mostra a mensagem se pediram
        printf("Arquivo '%s' criado vazio.\n", ARQUIVO);    // avisa que criou
    }                                           // fim do if
}

// Se o arquivo ainda nao existe, cria ele sozinho
void garantirArquivo(void) {
    FILE *arquivo = fopen(ARQUIVO, "r");        // tenta abrir para leitura
    if (arquivo == NULL) {                      // nao conseguiu abrir: o arquivo nao existe
        criarArquivo(0);                        // cria o arquivo sem mostrar mensagem
    } else {                                    // o arquivo existe
        fclose(arquivo);                        // so fecha, nao precisa fazer mais nada
    }                                           // fim do if/else
}

// LISTAR: mostra todos os produtos
void listarProdutos(void) {
    Produto lista[MAX];                         // vetor para guardar os produtos
    int n = carregarProdutos(lista);            // carrega do arquivo e guarda a quantidade
    if (n == 0) {                               // se nao tem nenhum produto...
        printf("Nenhum produto cadastrado.\n"); // ...avisa o usuario
        return;                                 // ...e sai da funcao
    }                                           // fim do if
    mostrarTabela(lista, n);                    // mostra a tabela com todos os produtos
}

// INSERIR: adiciona um produto no final do arquivo
void inserirProduto(void) {
    Produto novo;                               // produto que sera cadastrado

    lerTexto("Nome do produto: ", novo.produto, sizeof(novo.produto));  // pede o nome
    if (strlen(novo.produto) == 0) {            // se o nome ficou vazio...
        printf("Nome nao pode ser vazio.\n");   // ...avisa o usuario
        return;                                 // ...e cancela o cadastro
    }                                           // fim do if
    if (nomeDuplicado(novo.produto, 0)) {       // se ja existe produto com esse nome...
        printf("Ja existe um produto com esse nome. Cadastro cancelado.\n");    // ...avisa
        return;                                 // ...e cancela o cadastro
    }                                           // fim do if
    lerTexto("Categoria: ", novo.categoria, sizeof(novo.categoria));    // pede a categoria
    novo.preco      = lerReal("Preco (R$): ");                          // pede o preco
    novo.quantidade = lerInteiro("Quantidade: ");                       // pede a quantidade
    lerTexto("Fornecedor: ", novo.fornecedor, sizeof(novo.fornecedor)); // pede o fornecedor

    limparSeparador(novo.produto);              // tira ';' do nome
    limparSeparador(novo.categoria);            // tira ';' da categoria
    limparSeparador(novo.fornecedor);           // tira ';' do fornecedor

    novo.id = gerarId();                        // gera o ID automatico (maior + 1)

    FILE *arquivo = fopen(ARQUIVO, "a");        // "a" = acrescenta no final do arquivo
    if (arquivo == NULL) {                      // se deu erro ao abrir...
        printf("Erro ao abrir o arquivo!\n");   // ...avisa o usuario
        return;                                 // ...e sai da funcao
    }                                           // fim do if
    gravarProduto(arquivo, &novo);              // grava o novo produto no arquivo
    fclose(arquivo);                            // fecha o arquivo
    printf("Produto cadastrado com ID %d.\n", novo.id);     // mostra o ID gerado
}

// ALTERAR: copia tudo para um arquivo temporario, mudando so o produto escolhido
// No final o temporario substitui o original (evita corromper os dados)
void alterarProduto(void) {
    int id = lerInteiro("ID do produto a alterar: ");   // pede o ID do produto

    FILE *origem = fopen(ARQUIVO, "r");         // abre o arquivo original para leitura
    if (origem == NULL) {                       // se nao conseguiu abrir...
        printf("Arquivo nao encontrado.\n");    // ...avisa o usuario
        return;                                 // ...e sai da funcao
    }                                           // fim do if
    FILE *temp = fopen(TEMPORARIO, "w");        // cria o arquivo temporario para escrita
    if (temp == NULL) {                         // se deu erro ao criar...
        fclose(origem);                         // ...fecha o original
        printf("Erro ao criar o arquivo temporario.\n");    // ...avisa o usuario
        return;                                 // ...e sai da funcao
    }                                           // fim do if

    char linha[250];                            // linha lida do arquivo
    char digitado[100];                         // o que o usuario digita
    Produto p;                                  // produto da linha atual
    int achou = 0;                              // vira 1 se o ID for encontrado

    while (fgets(linha, sizeof(linha), origem) != NULL) {   // le o arquivo linha por linha
        if (!linhaParaProduto(linha, &p)) {     // se a linha estiver invalida...
            continue;                           // ...pula para a proxima
        }                                       // fim do if

        if (p.id == id) {                       // este e o produto que queremos alterar?
            achou = 1;                          // marca que encontrou
            printf("Produto atual: %s (deixe vazio para manter o valor)\n", p.produto); // mostra o produto atual

            lerTexto("Novo nome: ", digitado, sizeof(digitado));    // pede o novo nome
            if (nomeDuplicado(digitado, p.id) && strlen(digitado) > 0) {   // o nome novo ja existe em outro produto?
                printf("Nome ja existe em outro produto; mantido o atual.\n");  // avisa e nao troca
            } else {                                                        // nome livre (ou vazio)
                copiarSeNaoVazio(p.produto, digitado, sizeof(p.produto));   // troca o nome se digitou algo
            }                                                               // fim do if/else

            lerTexto("Nova categoria: ", digitado, sizeof(digitado));       // pede a nova categoria
            copiarSeNaoVazio(p.categoria, digitado, sizeof(p.categoria));   // troca se digitou algo

            lerTexto("Novo preco: ", digitado, sizeof(digitado));           // pede o novo preco
            if (strlen(digitado) > 0) {                                     // se digitou algo...
                for (char *q = digitado; *q != '\0'; q++) {                 // percorre o texto com ponteiro
                    if (*q == ',') *q = '.';                                // troca virgula por ponto
                }                                                           // fim do for
                p.preco = (float)atof(digitado);                            // converte para float e guarda
            }                                                               // fim do if

            lerTexto("Nova quantidade: ", digitado, sizeof(digitado));      // pede a nova quantidade
            if (strlen(digitado) > 0) {                                     // se digitou algo...
                p.quantidade = atoi(digitado);                              // converte para int e guarda
            }                                                               // fim do if

            lerTexto("Novo fornecedor: ", digitado, sizeof(digitado));      // pede o novo fornecedor
            copiarSeNaoVazio(p.fornecedor, digitado, sizeof(p.fornecedor)); // troca se digitou algo

            limparSeparador(p.produto);         // tira ';' do nome
            limparSeparador(p.categoria);       // tira ';' da categoria
            limparSeparador(p.fornecedor);      // tira ';' do fornecedor
        }                                       // fim do if (produto encontrado)
        gravarProduto(temp, &p);                // grava o produto (alterado ou nao) no temporario
    }                                           // fim do while
    fclose(origem);                             // fecha o arquivo original
    fclose(temp);                               // fecha o arquivo temporario

    remove(ARQUIVO);                            // apaga o arquivo original
    rename(TEMPORARIO, ARQUIVO);                // o temporario passa a ser o original

    if (achou) printf("Produto alterado com sucesso.\n");   // mensagem de sucesso
    else       printf("ID %d nao encontrado.\n", id);       // mensagem de erro
}

// EXCLUIR: copia para o temporario todos os produtos, menos o escolhido
void excluirProduto(void) {
    int id = lerInteiro("ID do produto a excluir: ");   // pede o ID do produto

    FILE *origem = fopen(ARQUIVO, "r");         // abre o arquivo original para leitura
    if (origem == NULL) {                       // se nao conseguiu abrir...
        printf("Arquivo nao encontrado.\n");    // ...avisa o usuario
        return;                                 // ...e sai da funcao
    }                                           // fim do if
    FILE *temp = fopen(TEMPORARIO, "w");        // cria o arquivo temporario para escrita
    if (temp == NULL) {                         // se deu erro ao criar...
        fclose(origem);                         // ...fecha o original
        printf("Erro ao criar o arquivo temporario.\n");    // ...avisa o usuario
        return;                                 // ...e sai da funcao
    }                                           // fim do if

    char linha[250];                            // linha lida do arquivo
    Produto p;                                  // produto da linha atual
    int achou = 0;                              // vira 1 se o ID for encontrado

    while (fgets(linha, sizeof(linha), origem) != NULL) {   // le o arquivo linha por linha
        if (!linhaParaProduto(linha, &p)) {     // se a linha estiver invalida...
            continue;                           // ...pula para a proxima
        }                                       // fim do if
        if (p.id == id) {                       // este e o produto a excluir?
            achou = 1;                          // marca que encontrou (NAO grava no temporario)
        } else {                                // nao e o produto a excluir
            gravarProduto(temp, &p);            // copia o produto para o temporario
        }                                       // fim do if/else
    }                                           // fim do while
    fclose(origem);                             // fecha o arquivo original
    fclose(temp);                               // fecha o arquivo temporario

    remove(ARQUIVO);                            // apaga o arquivo original
    rename(TEMPORARIO, ARQUIVO);                // o temporario passa a ser o original

    if (achou) printf("Produto excluido com sucesso.\n");   // mensagem de sucesso
    else       printf("ID %d nao encontrado.\n", id);       // mensagem de erro
}

// BUSCAR: por nome (1) ou por categoria (2), sem diferenciar maiuscula/minuscula
void buscarProdutos(void) {
    printf("Buscar por: 1 - Nome | 2 - Categoria\n");   // mostra as opcoes de busca
    int tipo = lerInteiro("Escolha: ");                 // le a opcao escolhida
    if (tipo != 1 && tipo != 2) {                       // se nao for 1 nem 2...
        printf("Opcao invalida.\n");                    // ...avisa o usuario
        return;                                         // ...e sai da funcao
    }                                                   // fim do if
    char termo[50];                                     // texto que sera procurado
    lerTexto("Digite o que procurar: ", termo, sizeof(termo));  // pede o texto da busca

    Produto lista[MAX];                         // todos os produtos do arquivo
    Produto achados[MAX];                       // so os produtos encontrados
    int n = carregarProdutos(lista);            // carrega os produtos do arquivo
    int qtd = 0;                                // quantos foram encontrados

    for (Produto *p = lista; p < lista + n; p++) {  // percorre a lista usando ponteiro
        int encontrou;                              // 1 se este produto combina com a busca
        if (tipo == 1) {                            // busca por nome
            encontrou = contemTexto(p->produto, termo);     // procura o termo no nome
        } else {                                    // busca por categoria
            encontrou = contemTexto(p->categoria, termo);   // procura o termo na categoria
        }                                           // fim do if/else
        if (encontrou) {                            // se o produto combina...
            achados[qtd] = *p;                      // ...copia ele para a lista de achados
            qtd++;                                  // ...e conta mais um
        }                                           // fim do if
    }                                               // fim do for
    if (qtd == 0) printf("Nenhum produto encontrado.\n");  // nada encontrado
    else          mostrarTabela(achados, qtd);              // mostra os encontrados
}

/* ============== ORDENACAO (Bubble Sort) ================ */

// Diz se os produtos "a" e "b" estao fora de ordem e precisam trocar de lugar
// opcao 1 = nome A-Z | 2 = nome Z-A | 3 = preco menor->maior | 4 = preco maior->menor
int precisaTrocar(const Produto *a, const Produto *b, int opcao) {
    if (opcao == 1) return compararTexto(a->produto, b->produto) > 0;  // A-Z: troca se "a" vem depois de "b"
    if (opcao == 2) return compararTexto(a->produto, b->produto) < 0;  // Z-A: troca se "a" vem antes de "b"
    if (opcao == 3) return a->preco > b->preco;                        // menor->maior: troca se "a" e mais caro
    return a->preco < b->preco;                                        // opcao 4, maior->menor: troca se "a" e mais barato
}

// Bubble Sort: compara vizinhos e troca os que estao fora de ordem
void bubbleSort(Produto *v, int n, int opcao) {
    Produto aux;                                    // guarda um produto durante a troca
    for (int i = 0; i < n - 1; i++) {               // varias passadas pelo vetor
        for (int j = 0; j < n - i - 1; j++) {       // compara cada par de vizinhos
            if (precisaTrocar(&v[j], &v[j + 1], opcao)) {   // o par esta fora de ordem?
                aux      = v[j];                    // guarda v[j] no auxiliar
                v[j]     = v[j + 1];                // v[j] recebe o proximo
                v[j + 1] = aux;                     // o proximo recebe o que foi guardado
            }                                       // fim do if
        }                                           // fim do for de dentro
    }                                               // fim do for de fora
}

// Carrega, ordena e mostra (o arquivo NAO e alterado)
void ordenarEMostrar(int opcao, const char *titulo) {
    Produto lista[MAX];                             // vetor em memoria
    int n = carregarProdutos(lista);                // carrega os produtos do arquivo
    if (n == 0) {                                   // se nao tem nada para ordenar...
        printf("Nenhum produto cadastrado.\n");     // ...avisa o usuario
        return;                                     // ...e sai da funcao
    }                                               // fim do if
    bubbleSort(lista, n, opcao);                    // ordena so o vetor (nao o arquivo)
    printf("\n>>> Ordenado por %s\n", titulo);      // mostra como foi ordenado
    mostrarTabela(lista, n);                        // mostra a tabela ordenada
}

/* ===================== EXPORTAR PARA CSV ===================== */

// Cria o arquivo loja.csv (campos separados por virgula)
void exportarCSV(void) {
    Produto lista[MAX];                             // vetor em memoria
    int n = carregarProdutos(lista);                // carrega os produtos do arquivo
    if (n == 0) {                                   // se nao tem nada para exportar...
        printf("Nada para exportar.\n");            // ...avisa o usuario
        return;                                     // ...e sai da funcao
    }                                               // fim do if
    FILE *arquivo = fopen(CSV, "w");                // cria o arquivo CSV
    if (arquivo == NULL) {                          // se deu erro ao criar...
        printf("Erro ao criar o CSV.\n");           // ...avisa o usuario
        return;                                     // ...e sai da funcao
    }                                               // fim do if
    fprintf(arquivo, "ID,Produto,Categoria,Preco,Quantidade,Fornecedor\n");     // escreve o cabecalho
    for (Produto *p = lista; p < lista + n; p++) {  // percorre os produtos usando ponteiro
        fprintf(arquivo, "%d,\"%s\",\"%s\",%.2f,%d,\"%s\"\n",   // uma linha por produto (textos entre aspas)
                p->id, p->produto, p->categoria,                // id, nome e categoria
                p->preco, p->quantidade, p->fornecedor);        // preco, quantidade e fornecedor
    }                                               // fim do for
    fclose(arquivo);                                // fecha o CSV
    printf("Exportado %d produto(s) para '%s'.\n", n, CSV);     // avisa que exportou

    // O OneCompiler nao deixa baixar arquivos, entao mostramos o CSV na tela
    printf("--- Conteudo de %s ---\n", CSV);        // titulo do conteudo
    FILE *leitura = fopen(CSV, "r");                // abre o CSV para leitura
    char linha[250];                                // guarda cada linha lida
    while (leitura != NULL && fgets(linha, sizeof(linha), leitura) != NULL) {   // le linha por linha
        printf("%s", linha);                        // mostra a linha na tela
    }                                               // fim do while
    if (leitura != NULL) fclose(leitura);           // fecha o arquivo se foi aberto
}

/* ===================== PROGRAMA PRINCIPAL ===================== */

int main(void) {
    int opcao;                                      // opcao escolhida no menu
    garantirArquivo();                              // cria o loja.txt se ele nao existir

    do {                                            // repete o menu ate escolher 0
        printf("\n========== ELECTROVENDAS ==========\n");    // titulo do menu
        printf(" 1 - Criar arquivo (vazio)\n");                     // opcao 1
        printf(" 2 - Listar produtos\n");                           // opcao 2
        printf(" 3 - Inserir produto\n");                           // opcao 3
        printf(" 4 - Alterar produto\n");                           // opcao 4
        printf(" 5 - Excluir produto\n");                           // opcao 5
        printf(" 6 - Buscar (nome ou categoria)\n");                // opcao 6
        printf(" 7 - Ordenar por nome (A-Z)\n");                    // opcao 7
        printf(" 8 - Ordenar por nome (Z-A)\n");                    // opcao 8
        printf(" 9 - Ordenar por preco (menor -> maior)\n");        // opcao 9
        printf("10 - Ordenar por preco (maior -> menor)\n");        // opcao 10
        printf("11 - Exportar para CSV\n");                         // opcao 11
        printf(" 0 - Sair\n");                                      // opcao 0
        fflush(stdout);                             // forca o menu a aparecer na hora

        opcao = lerInteiro("Escolha: ");            // le a opcao do usuario
        if (feof(stdin)) break;                     // se acabou a entrada, encerra o programa

        switch (opcao) {                            // escolhe o que fazer conforme a opcao
            case 1:  criarArquivo(1);   break;      // recria o arquivo vazio
            case 2:  listarProdutos();  break;      // lista os produtos
            case 3:  inserirProduto();  break;      // insere um produto novo
            case 4:  alterarProduto();  break;      // altera um produto
            case 5:  excluirProduto();  break;      // exclui um produto
            case 6:  buscarProdutos();  break;      // busca por nome ou categoria
            case 7:  ordenarEMostrar(1, "nome (A-Z)");              break;  // ordena por nome A-Z
            case 8:  ordenarEMostrar(2, "nome (Z-A)");              break;  // ordena por nome Z-A
            case 9:  ordenarEMostrar(3, "preco (menor -> maior)");  break;  // ordena por preco crescente
            case 10: ordenarEMostrar(4, "preco (maior -> menor)");  break;  // ordena por preco decrescente
            case 11: exportarCSV();     break;      // exporta para CSV
            case 0:  printf("Saindo...\n"); break;  // mensagem de saida
            default: printf("Opcao invalida!\n");   // qualquer outro numero e invalido
        }                                           // fim do switch
    } while (opcao != 0);                           // volta ao menu enquanto nao for 0

    return 0;                                       // fim do programa (0 = deu tudo certo)
}
