/* detective.c
 * Detective Quest - Mapa da mansão representado por árvore binária
 *
 * Requisitos:
 *  - criarSala(): cria dinamicamente uma sala com nome
 *  - explorarSalas(): permite navegação interativa (e, d, s)
 *  - main(): monta o mapa e inicia a exploração
 *
 * Autor: Enigma Studios (exemplo didático)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Estrutura que representa uma sala (nó da árvore binária) */
typedef struct Sala {
    char *nome;           /* nome da sala */
    struct Sala *esq;     /* ponteiro para sala à esquerda */
    struct Sala *dir;     /* ponteiro para sala à direita */
} Sala;

/* Função auxiliar: duplicar string (substitui strdup para portabilidade) */
char *strdup_port(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (!p) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memcpy(p, s, n);
    return p;
}

/* criarSala() – cria, de forma dinâmica, uma sala com nome.
 * Parâmetro: nome (const char*) - nome da sala
 * Retorna: ponteiro para Sala alocada dinamicamente
 */
Sala *criarSala(const char *nome) {
    Sala *nova = (Sala *) malloc(sizeof(Sala));
    if (!nova) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    nova->nome = strdup_port(nome);
    nova->esq = NULL;
    nova->dir = NULL;
    return nova;
}

/* liberarSalas() - libera memória da árvore (pós-ordem) */
void liberarSalas(Sala *raiz) {
    if (!raiz) return;
    liberarSalas(raiz->esq);
    liberarSalas(raiz->dir);
    free(raiz->nome);
    free(raiz);
}

/* explorarSalas() – permite a navegação do jogador pela árvore.
 * Inicia a partir da sala passada em 'inicio'.
 * O jogador digita:
 *  - 'e' para esquerda
 *  - 'd' para direita
 *  - 's' para sair da exploração
 *
 * Durante a exploração, os nomes das salas visitadas são
 * armazenados e, ao final, exibimos o caminho percorrido.
 */
void explorarSalas(Sala *inicio) {
    if (!inicio) {
        printf("Mapa vazio. Não há salas para explorar.\n");
        return;
    }

    /* Vetor dinâmico para armazenar ponteiros para nomes das salas visitadas */
    size_t capacidade = 8;
    size_t cont = 0;
    char **visitadas = malloc(capacidade * sizeof(char *));
    if (!visitadas) { perror("malloc"); exit(EXIT_FAILURE); }

    Sala *atual = inicio;

    /* Registrar a sala inicial como visitada */
    if (cont + 1 > capacidade) {
        capacidade *= 2;
        visitadas = realloc(visitadas, capacidade * sizeof(char *));
        if (!visitadas) { perror("realloc"); exit(EXIT_FAILURE); }
    }
    visitadas[cont++] = atual->nome;

    printf("Você está no Hall de entrada: %s\n", atual->nome);

    /* Loop de exploração até chegar num nó-folha (sem filhos) ou o jogador sair */
    while (1) {
        /* Se atual é folha (não tem caminhos), encerramos a exploração automaticamente */
        if (atual->esq == NULL && atual->dir == NULL) {
            printf("\nVocê chegou a um cômodo sem saídas (%s).\n", atual->nome);
            break;
        }

        /* Mostrar opções disponíveis (somente as que existem) */
        printf("\nVocê está na sala: %s\n", atual->nome);
        printf("Escolha um caminho:\n");
        if (atual->esq) printf("  (e) Esquerda -> %s\n", atual->esq->nome);
        if (atual->dir) printf("  (d) Direita  -> %s\n", atual->dir->nome);
        printf("  (s) Sair da exploração\n");
        printf("Digite sua escolha (e/d/s): ");

        char entrada[32];
        if (!fgets(entrada, sizeof(entrada), stdin)) {
            /* EOF ou erro de leitura: sair */
            printf("\nEntrada encerrada. Saindo...\n");
            break;
        }

        /* Pegar primeira letra útil e minúscula */
        char opc = '\0';
        for (size_t i = 0; i < strlen(entrada); ++i) {
            if (!isspace((unsigned char)entrada[i])) { opc = tolower((unsigned char)entrada[i]); break; }
        }
        if (opc == '\0') {
            printf("Entrada inválida. Tente novamente.\n");
            continue;
        }

        if (opc == 's') {
            printf("Você optou por sair da exploração.\n");
            break;
        } else if (opc == 'e') {
            if (atual->esq) {
                atual = atual->esq;
            } else {
                printf("Não há caminho à esquerda nesta sala. Escolha outra opção.\n");
                continue;
            }
        } else if (opc == 'd') {
            if (atual->dir) {
                atual = atual->dir;
            } else {
                printf("Não há caminho à direita nesta sala. Escolha outra opção.\n");
                continue;
            }
        } else {
            printf("Opção inválida. Use 'e', 'd' ou 's'.\n");
            continue;
        }

        /* Registra a sala visitada */
        if (cont + 1 > capacidade) {
            capacidade *= 2;
            visitadas = realloc(visitadas, capacidade * sizeof(char *));
            if (!visitadas) { perror("realloc"); exit(EXIT_FAILURE); }
        }
        visitadas[cont++] = atual->nome;

        /* Exibe imediatamente a sala visitada */
        printf("Você entrou em: %s\n", atual->nome);
    }

    /* Exibir o caminho percorrido (nomes das salas visitadas em ordem) */
    printf("\n--- Caminho percorrido ---\n");
    for (size_t i = 0; i < cont; ++i) {
        printf("%s", visitadas[i]);
        if (i + 1 < cont) printf(" -> ");
    }
    printf("\n-------------------------\n");

    free(visitadas);
}

/* main() – monta o mapa inicial e dá início à exploração. */
int main(void) {
    /* Montagem manual da árvore (mapa da mansão).
     * Exemplo de estrutura (substitua ou expanda conforme desejar):
     *
     *                          Hall
     *                         /    \
     *                   Sala de   Biblioteca
     *                    estar     /      \
     *                            Escritório Jardim
     *                           /
     *                      Porão
     *
     * Nós-folha: Sala de estar (se não tiver filhos), Porão, Jardim
     */

    /* Criando salas (alocação dinâmica) */
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *escritorio = criarSala("Escritório");
    Sala *jardim = criarSala("Jardim Interno");
    Sala *porao = criarSala("Porão Misterioso");
    Sala *cozinha = criarSala("Cozinha Antiga"); /* exemplo extra */
    Sala *sotao = criarSala("Sótão empoeirado"); /* exemplo extra */

    /* Montagem das ligações (filhos esquerdo/direito) */
    hall->esq = salaEstar;           /* hall esquerda -> sala de estar */
    hall->dir = biblioteca;          /* hall direita -> biblioteca */

    salaEstar->esq = cozinha;        /* sala de estar -> cozinha (esq) */
    salaEstar->dir = NULL;           /* sem caminho à direita (exemplo) */

    biblioteca->esq = escritorio;    /* biblioteca esquerda -> escritório */
    biblioteca->dir = jardim;        /* biblioteca direita -> jardim */

    escritorio->esq = porao;         /* escritório esquerda -> porão */
    escritorio->dir = sotao;         /* escritório direita -> sótão */

    cozinha->esq = NULL;
    cozinha->dir = NULL;

    porao->esq = NULL;
    porao->dir = NULL;

    jardim->esq = NULL;
    jardim->dir = NULL;

    sotao->esq = NULL;
    sotao->dir = NULL;

    /* Mensagem inicial e início da exploração */
    printf("Bem-vindo(a) ao Detective Quest - exploração da mansão.\n");
    printf("Você começará no Hall de entrada. Explore escolhendo 'e' (esquerda), 'd' (direita) ou 's' (sair).\n\n");

    explorarSalas(hall);

    /* Liberar memória antes de encerrar */
    liberarSalas(hall);

    printf("\nExploração encerrada. Obrigado por jogar Detective Quest (demo).\n");
    return 0;
}
