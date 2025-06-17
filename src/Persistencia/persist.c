#include "Persistencia/persist.h"
#include "Evento/eventos.h"
#include "Utils/utils.h"
#include "Participantes/participantes.h"
#include "Inscricoes/inscricoes.h"

void salvarEventosCSV() {
    FILE *file = fopen("data/eventos.csv", "w");
    if (!file) {
        printf(RED "Erro ao salvar eventos em data/eventos.csv!\n" RESET);
        return;
    }
    fprintf(file, "ID,Nome,Categoria,Vagas,MaxVagas\n");
    for (Evento *e = eventos; e != NULL; e = e->prox) {
        fprintf(file, "%d,%s,%s,%d,%d\n", e->id, e->nome, e->categoria, e->vagas, e->max_vagas);
    }
    fclose(file);
}

void salvarParticipantesCSV() {
    FILE *file = fopen("data/participantes.csv", "w");
    if (!file) {
        printf(RED "Erro ao salvar participantes em data/participantes.csv!\n" RESET);
        return;
    }
    fprintf(file, "ID,Nome,Email\n");
    for (int i = 0; i < HASH_SIZE; i++) {
        if (hash_table[i] != NULL) {
            fprintf(file, "%d,%s,%s\n", hash_table[i]->id, hash_table[i]->nome, hash_table[i]->email);
        }
    }
    fclose(file);
}

void salvarInscricoesCSV() {
    FILE *file = fopen("data/inscricoes.csv", "w");
    if (!file) {
        printf(RED "Erro ao salvar inscricoes em data/inscricoes.csv!\n" RESET);
        return;
    }
    fprintf(file, "EventoID,ParticipanteID,Nome,Email\n");
    for (Inscricao *i = todas_inscricoes; i != NULL; i = i->prox) {
        fprintf(file, "%d,%d,%s,%s\n", i->evento_id, i->participante_id, i->nome_participante, i->email_participante);
    }
    fclose(file);
}

void salvarFilaEsperaCSV() {
    FILE *file = fopen("data/fila_espera.csv", "w");
    if (!file) {
        printf(RED "Erro ao salvar fila de espera em data/fila_espera.csv!\n" RESET);
        return;
    }
    fprintf(file, "EventoID,ParticipanteID\n");
    for (FilaEspera *f = inicio_fila; f != NULL; f = f->prox) {
        fprintf(file, "%d,%d\n", f->evento_id, f->participante->id);
    }
    fclose(file);
}

void salvarTudoCSV() {
    salvarEventosCSV();
    salvarParticipantesCSV();
    salvarInscricoesCSV();
    salvarFilaEsperaCSV();
    printf(GREEN "Todos os dados foram salvos em arquivos CSV!\n" RESET);
}

void carregarEventosCSV() {
    FILE *file = fopen("data/eventos.csv", "r");
    if (!file) return;

    char linha[MAX * 4 + 20];
    fgets(linha, sizeof(linha), file);

    while (fgets(linha, sizeof(linha), file)) {
        Evento *novo = malloc(sizeof(Evento));
        if (!novo) continue;
        novo->participantes = NULL;
        novo->prox = NULL;

        linha[strcspn(linha, "\n")] = 0;

        int campos_lidos = sscanf(linha, "%d,%49[^,],%49[^,],%d,%d",
                                  &novo->id, novo->nome, novo->categoria, &novo->vagas, &novo->max_vagas);
        if (campos_lidos < 4) {
            printf(RED "Erro ao ler linha de evento: %s\n" RESET, linha);
            free(novo);
            continue;
        }
        if (campos_lidos == 4) {
            novo->max_vagas = VAGAS_POR_EVENTO;
        }

        Evento *existente = eventos;
        while (existente && existente->id != novo->id) existente = existente->prox; // <-- CORRIGIDO AQUI!
        if (existente) {
            free(novo);
            continue;
        }

        if (eventos == NULL) eventos = novo;
        else {
            Evento *atual = eventos;
            while (atual->prox != NULL) atual = atual->prox;
            atual->prox = novo;
        }
        if (novo->id >= next_evento_id) next_evento_id = novo->id + 1;
        total_eventos++;
    }
    fclose(file);
}

void carregarParticipantesCSV() {
    FILE *file = fopen("data/participantes.csv", "r");
    if (!file) return;

    char linha[MAX * 3 + 10];
    fgets(linha, sizeof(linha), file);

    while (fgets(linha, sizeof(linha), file)) {
        Part *novo = malloc(sizeof(Part));
        if (!novo) continue;
        novo->prox = NULL;

        linha[strcspn(linha, "\n")] = 0;

        if (sscanf(linha, "%d,%49[^,],%49s", &novo->id, novo->nome, novo->email) != 3) {
             printf(RED "Erro ao ler linha de participante: %s\n" RESET, linha);
             free(novo);
             continue;
        }

        int index = hashParticipante(novo->email);
        if (hash_table[index] == NULL) {
            hash_table[index] = novo;
            if (novo->id >= next_part_id) next_part_id = novo->id + 1;
        } else {
             if (strcmp(hash_table[index]->email, novo->email) == 0) {
                free(novo);
            } else {
                printf(YELLOW "Colisao de hash nao tratada para email %s (carregamento). Participante nao carregado do CSV para evitar sobrescrita.\n" RESET, novo->email);
                free(novo);
             }
        }
    }
    fclose(file);
}

void carregarInscricoesCSV() {
    FILE *file = fopen("data/inscricoes.csv", "r");
    if (!file) return;

    char linha[MAX * 4 + 20];
    fgets(linha, sizeof(linha), file);

    while (fgets(linha, sizeof(linha), file)) {
        Inscricao *nova = malloc(sizeof(Inscricao));
        if (!nova) continue;

        linha[strcspn(linha, "\n")] = 0;

        if (sscanf(linha, "%d,%d,%49[^,],%49s", &nova->evento_id, &nova->participante_id, nova->nome_participante, nova->email_participante) != 4) {
            printf(RED "Erro ao ler linha de inscricao: %s\n" RESET, linha);
            free(nova);
            continue;
        }

        nova->prox = todas_inscricoes;
        todas_inscricoes = nova;

        Evento *e = eventos;
        while(e && e->id != nova->evento_id) e = e->prox;

        Part *p = NULL;
        p = buscaParticipantePorEmailNaHash(nova->email_participante);

        if(e && p){
            int ja_no_evento = 0;
            if(e->participantes){
                Part * atual_part_evento = e->participantes->prox;
                do {
                    if(atual_part_evento == p) {
                        ja_no_evento = 1;
                        break;
                    }
                    if (atual_part_evento == e->participantes) break;
                    atual_part_evento = atual_part_evento->prox;
                } while (e->participantes != NULL && atual_part_evento != e->participantes->prox);
            }

            if(!ja_no_evento){
                 if (!e->participantes) {
                    e->participantes = p;
                    p->prox = p;
                } else {
                    p->prox = e->participantes->prox;
                    e->participantes->prox = p;
                    e->participantes = p;
                }
                // REMOVIDO: e->vagas--;
            }
        }
    }
    fclose(file);
}

void carregarFilaEsperaCSV() {
    FILE *file = fopen("data/fila_espera.csv", "r");
    if (!file) return;

    char linha[MAX + 10];
    fgets(linha, sizeof(linha), file);

    while (fgets(linha, sizeof(linha), file)) {
        int evento_id_f, part_id_f;
        linha[strcspn(linha, "\n")] = 0;

        if (sscanf(linha, "%d,%d", &evento_id_f, &part_id_f) != 2) {
            printf(RED "Erro ao ler linha da fila de espera: %s\n" RESET, linha);
            continue;
        }

        Part *p = NULL;
        for (int k = 0; k < HASH_SIZE; k++) {
            if (hash_table[k] && hash_table[k]->id == part_id_f) {
                p = hash_table[k];
                break;
            }
        }
        if (!p) {
            printf(YELLOW "Participante ID %d da fila de espera nao encontrado na hash. Pode ter sido removido ou nao carregado.\n" RESET, part_id_f);
            continue;
        }

        Evento *e_check = eventos;
        while(e_check && e_check->id != evento_id_f) e_check = e_check->prox;
        if(!e_check){
            printf(YELLOW "Evento ID %d da fila de espera nao encontrado. Pode ter sido removido ou nao carregado.\n" RESET, evento_id_f);
            continue;
        }

        FilaEspera *nova_f = malloc(sizeof(FilaEspera));
        if (!nova_f) continue;

        nova_f->evento_id = evento_id_f;
        nova_f->participante = p;
        nova_f->prox = NULL;

        if (!inicio_fila) inicio_fila = fim_fila = nova_f;
        else { fim_fila->prox = nova_f;
        fim_fila = nova_f; }
    }
    fclose(file);
}

void carregarTudoCSV() {
    carregarEventosCSV();
    carregarParticipantesCSV();
    carregarInscricoesCSV();
    carregarFilaEsperaCSV();
    atualizarIndice();
    printf(GREEN "Dados carregados dos arquivos CSV!\n" RESET);
}

void liberarMemoria() {
    Evento *e = eventos;
    while (e != NULL) {
        Evento *temp_e = e;
        e = e->prox;

        if (temp_e->participantes != NULL) {
            temp_e->participantes->prox = NULL;
        }
        free(temp_e);
    }
    eventos = NULL;

    Inscricao *i = todas_inscricoes;
    while (i != NULL) {
        Inscricao *temp_i = i;
        i = i->prox;
        free(temp_i);
    }
    todas_inscricoes = NULL;

    FilaEspera *f = inicio_fila;
    while (f != NULL) {
        FilaEspera *temp_f = f;
        f = f->prox;
        free(temp_f);
    }
    inicio_fila = fim_fila = NULL;

    for (int k = 0; k < HASH_SIZE; k++) {
        if (hash_table[k] != NULL) {
            free(hash_table[k]);
            hash_table[k] = NULL;
        }
    }

    while (topo_pilha) {
        PilhaEventos *temp_pilha = topo_pilha;
        topo_pilha = topo_pilha->prox;
        free(temp_pilha);
    }
    topo_pilha = NULL;

    if (indice_nome) {
        free(indice_nome);
        indice_nome = NULL;
    }
    total_indice = 0;

    total_eventos = 0;
    next_evento_id = 1;
    next_part_id = 1;
}
