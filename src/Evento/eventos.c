#include "Evento/eventos.h"
#include "Utils/utils.h"

void cadastrarEvento() {
    if (total_eventos >= MAX_EVENTOS) {
        printf(RED "\nLimite maximo de eventos atingido!\n" RESET);
        return;
    }

    char nome_evento[MAX];
    obterStringSegura(nome_evento, MAX, "\nNome do evento: ");

    Evento *e_existente = buscaIndexada(nome_evento);
    if (!e_existente) {
        e_existente = eventos;
        while (e_existente != NULL) {
            if (strcmp(e_existente->nome, nome_evento) == 0) {
                break;
            }
            e_existente = e_existente->prox;
        }
    }

    if (e_existente != NULL) {
        printf(RED "\nJa existe um evento com o nome '%s' cadastrado!\n" RESET, nome_evento);
        return;
    }

    Evento *novo = malloc(sizeof(Evento));
    if (!novo) {
        printf(RED "\nErro de alocacao!\n" RESET);
        return;
    }
    novo->id = next_evento_id++;
    novo->prox = NULL;
    novo->vagas = VAGAS_POR_EVENTO;
    novo->max_vagas = VAGAS_POR_EVENTO;
    novo->participantes = NULL;
    strcpy(novo->nome, nome_evento);

    printf("Categoria: ");
    obterStringSegura(novo->categoria, MAX, "");

    if (!eventos) eventos = novo;
    else {
        Evento *atual = eventos;
        while (atual->prox) atual = atual->prox;
        atual->prox = novo;
    }
    total_eventos++;
    atualizarIndice();
    bubbleSort();
    printf(GREEN "\nEvento %s cadastrado com sucesso!\n" RESET, novo->nome);
}

void listarEventos() {
    printf("\n" BLUE "===================================\n");
    printf("        LISTA DE EVENTOS         \n");
    printf("===================================\n" RESET);

    if (!eventos) {
        printf("\n" YELLOW "Nenhum evento cadastrado ainda!\n" RESET);
        printf("\n" BLUE "-----------------------------------\n" RESET);
        return;
    }

    for (Evento *e = eventos; e; e = e->prox) {
        printf("\n> Evento: %s\n", e->nome);
        printf("> Categoria: %s\n", e->categoria);
        int num_participantes = e->max_vagas - e->vagas; // <-- Variável declarada aqui!
        printf("> Vagas ocupadas: %d/%d\n", num_participantes, e->max_vagas);
        printf("\n" BLUE "-----------------------------------\n" RESET);
    }
}

void bubbleSort() {
    if (!eventos || !eventos->prox) return;
    int trocado;
    Evento **ptr, *temp, *ultimo = NULL;
    do {
        trocado = 0;
        ptr = &eventos;
        while ((*ptr)->prox != ultimo) {
            int cmp_cat = strcmp((*ptr)->categoria, (*ptr)->prox->categoria);
            int cmp_nome = strcmp((*ptr)->nome, (*ptr)->prox->nome);
            if (cmp_cat > 0 || (cmp_cat == 0 && cmp_nome > 0)) {
                temp = *ptr;
                *ptr = (*ptr)->prox;
                temp->prox = (*ptr)->prox;
                (*ptr)->prox = temp;
                trocado = 1;
            }
            ptr = &(*ptr)->prox;
        }
        ultimo = *ptr;
    } while (trocado);
}

Evento* merge(Evento *a, Evento *b) {
    if (!a) return b;
    if (!b) return a;

    if (strcmp(a->nome, b->nome) < 0) {
        a->prox = merge(a->prox, b);
        return a;
    } else {
        b->prox = merge(a, b->prox);
        return b;
    }
}

void split(Evento *source, Evento **front, Evento **back) {
    Evento *slow = source;
    Evento *fast = source->prox;

    while (fast != NULL) {
        fast = fast->prox;
        if (fast != NULL) {
            slow = slow->prox;
            fast = fast->prox;
        }
    }

    *front = source;
    *back = slow->prox;
    slow->prox = NULL;
}

void mergeSort(Evento **headRef) {
    Evento *head = *headRef;
    Evento *a, *b;

    if ((head == NULL) || (head->prox == NULL)) {
        return;
    }

    split(head, &a, &b);

    mergeSort(&a);
    mergeSort(&b);

    *headRef = merge(a, b);
}

void atualizarIndice() {
    if (indice_nome) {
        free(indice_nome);
        indice_nome = NULL;
    }

    if (total_eventos == 0) {
        total_indice = 0;
        return;
    }
    indice_nome = malloc(total_eventos * sizeof(Evento*));
    if (!indice_nome) {
        printf(RED "Erro ao alocar memoria para o indice de eventos!\n" RESET);
        total_indice = 0;
        return;
    }

    Evento *e = eventos;
    for (int i = 0; i < total_eventos && e; i++) {
        indice_nome[i] = e;
        e = e->prox;
    }
    total_indice = total_eventos;

    for (int i = 0; i < total_indice - 1; i++) {
        for (int j = 0; j < total_indice - i - 1; j++) {
            if (strcmp(indice_nome[j]->nome, indice_nome[j+1]->nome) > 0) {
                Evento *temp = indice_nome[j];
                indice_nome[j] = indice_nome[j+1];
                indice_nome[j+1] = temp;
            }
        }
    }
}

Evento* buscaIndexada(char *nome) {
    if (!indice_nome || total_indice == 0) return NULL;

    int inicio = 0;
    int fim = total_indice - 1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        int cmp = strcmp(nome, indice_nome[meio]->nome);

        if (cmp == 0) return indice_nome[meio];
        if (cmp < 0) fim = meio - 1;
        else inicio = meio + 1;
    }
    return NULL;
}
