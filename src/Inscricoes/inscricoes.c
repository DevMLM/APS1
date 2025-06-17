#include "Inscricoes/inscricoes.h"
#include "Evento/eventos.h"
#include "Utils/utils.h" // Para system("cls") e obterStringSegura
#include "Participantes/participantes.h" // Para buscaParticipantePorEmailNaHash
#include "FilaPilha/filaPilha.h"

int buscaParticipanteEmEvento(Part *lista_participantes_evento, const char *email) {
    if (!lista_participantes_evento || !email)
        return 0;

    Part *atual = lista_participantes_evento->prox; // Começa pelo "head" da lista circular
    
    do {
        if (strcmp(atual->email, email) == 0)
            return 1;
        atual = atual->prox;
    } while (atual != lista_participantes_evento->prox);

    return 0;
}

int inscricaoGlobalExiste(int evento_id, int participante_id) {
    if (evento_id <= 0 || participante_id <= 0)
        return 0;

    Inscricao *atual = todas_inscricoes;
    while (atual != NULL) {
        if (atual->evento_id == evento_id && atual->participante_id == participante_id) {
            return 1;
        }
        atual = atual->prox;
    }
    return 0;
}

void insereParticipante(Evento *e, Part *p) {
    if (!e || !p) {
        printf(RED "Erro: Evento ou participante inválido!\n" RESET);
        return;
    }

    // system("cls"); // Removido daqui - limpeza de tela deve ser na interface

    if (buscaParticipanteEmEvento(e->participantes, p->email)) {
        printf(RED "Você já está inscrito neste evento!\n" RESET);
        return;
    }
    
    if (inscricaoGlobalExiste(e->id, p->id)) {
        printf(RED "Você já tem uma inscrição global registrada para este evento!\n" RESET);
        return;
    }

    if (e->vagas <= 0) {
        printf(YELLOW "Evento lotado! %s (%s) adicionado à fila de espera.\n" RESET,
               p->nome, p->email);
        insereFilaEspera(e, p);
        return;
    }

    if (!e->participantes) {
        e->participantes = p;
        p->prox = p;
    } else {
        p->prox = e->participantes->prox;
        e->participantes->prox = p;
        e->participantes = p;
    }

    e->vagas--;

    printf(GREEN "%s (%s) adicionado ao evento %s! Vagas restantes: %d/%d\n" RESET,
           p->nome, p->email, e->nome, e->vagas, e->max_vagas);

    Inscricao *nova_insc = malloc(sizeof(Inscricao));
    if (!nova_insc) {
        printf(RED "Erro ao registrar inscrição global! (Falta de memória)\n" RESET);
        return;
    }
    nova_insc->evento_id = e->id;
    nova_insc->participante_id = p->id;
    strncpy(nova_insc->nome_participante, p->nome, MAX - 1);
    nova_insc->nome_participante[MAX - 1] = '\0';
    strncpy(nova_insc->email_participante, p->email, MAX - 1);
    nova_insc->email_participante[MAX - 1] = '\0';
    
    nova_insc->prox = todas_inscricoes;
    todas_inscricoes = nova_insc;

    pushEvento(e);
}

int efetuarRemocaoInscricao(Evento *evento_alvo, const char *email_participante) {
    if (!evento_alvo || !email_participante) {
        printf(RED "Erro: Parâmetros inválidos na remoção.\n" RESET);
        return 0;
    }

    if (!evento_alvo->participantes) {
        printf(YELLOW "Nenhum participante inscrito no evento %s para remover.\n" RESET, evento_alvo->nome);
        return 0;
    }

    Part *participante_a_remover = NULL;
    Part *atual_p = evento_alvo->participantes->prox;
    Part *anterior_p = evento_alvo->participantes;

    int encontrado = 0;
    do {
        if (strcmp(atual_p->email, email_participante) == 0) {
            participante_a_remover = atual_p;
            encontrado = 1;
            break;
        }
        anterior_p = atual_p;
        atual_p = atual_p->prox;
    } while (atual_p != evento_alvo->participantes->prox);

    if (!encontrado) {
        printf(RED "Participante %s não encontrado na lista de inscritos do evento %s.\n" RESET,
               email_participante, evento_alvo->nome);
        return 0;
    }

    if (participante_a_remover == evento_alvo->participantes && participante_a_remover->prox == participante_a_remover) {
        evento_alvo->participantes = NULL;
    } else {
        anterior_p->prox = participante_a_remover->prox;
        if (evento_alvo->participantes == participante_a_remover) {
            evento_alvo->participantes = anterior_p;
        }
    }

    evento_alvo->vagas++;
    printf(GREEN "Participante %s removido do evento %s. Vagas liberadas: %d/%d\n" RESET,
           email_participante, evento_alvo->nome, evento_alvo->vagas, evento_alvo->max_vagas);

    Inscricao *atual_i = todas_inscricoes;
    Inscricao *anterior_i = NULL;
    int removido_globalmente = 0;
    while (atual_i != NULL) {
        if (atual_i->evento_id == evento_alvo->id &&
            strcmp(atual_i->email_participante, email_participante) == 0) {
            
            if (anterior_i) {
                anterior_i->prox = atual_i->prox;
            } else {
                todas_inscricoes = atual_i->prox;
            }
            printf(GREEN "Inscrição global de %s no evento %s removida.\n" RESET,
                   atual_i->nome_participante, evento_alvo->nome);
            free(atual_i);
            removido_globalmente = 1;
            break;
        }
        anterior_i = atual_i;
        atual_i = atual_i->prox;
    }
    if (!removido_globalmente) {
        printf(RED "Erro: Inscrição global não encontrada para %s no evento %s.\n" RESET,
               email_participante, evento_alvo->nome);
    }

    FilaEspera *atual_f = inicio_fila;
    FilaEspera *anterior_f = NULL;
    while (atual_f != NULL) {
        if (atual_f->evento_id == evento_alvo->id) {
            Part *participante_promovido = atual_f->participante;

            if (anterior_f) {
                anterior_f->prox = atual_f->prox;
            } else {
                inicio_fila = atual_f->prox;
            }
            if (fim_fila == atual_f) {
                fim_fila = anterior_f;
            }
            
            printf(YELLOW "Promovendo %s da fila de espera para o evento %s.\n" RESET,
                   participante_promovido->nome, evento_alvo->nome);

            insereParticipante(evento_alvo, participante_promovido);
            
            free(atual_f);
            break;
        }
        anterior_f = atual_f;
        atual_f = atual_f->prox;
    }

    return 1;
}

void removerInscricaoUsuario(const char *email_usuario) {
    if (!email_usuario) return;

    if (!todas_inscricoes) {
        printf(RED "\nNenhuma inscrição para remover!\n" RESET);
        return;
    }

    printf(BLUE "\n=== SUAS INSCRIÇÕES ===\n" RESET);
    Inscricao *temp = todas_inscricoes;
    Evento *eventos_inscrito[MAX_EVENTOS];
    int count = 0;
    int opcoes[MAX_EVENTOS];
    int opcao_display = 1;

    while (temp != NULL && count < MAX_EVENTOS) {
        if (strcmp(temp->email_participante, email_usuario) == 0) {
            Evento *e = eventos;
            while (e && e->id != temp->evento_id)
                e = e->prox;
            if (e) {
                printf(CYAN "%d. %s (%s)\n" RESET, opcao_display, e->nome, e->categoria);
                eventos_inscrito[count] = e;
                opcoes[count] = opcao_display;
                count++;
                opcao_display++;
            }
        }
        temp = temp->prox;
    }

    if (count == 0) {
        printf(YELLOW "\nVocê não possui inscrições ativas.\n" RESET);
        return;
    }

    // Usar obterInteiroSeguro para leitura mais robusta
    printf(CYAN "\nEscolha o evento para remover (0 para cancelar): " RESET);
    int escolha;
    if (scanf("%d", &escolha) != 1) {
        limparBuffer();
        printf(RED "Entrada inválida!\n" RESET);
        return;
    }
    limparBuffer();

    if (escolha == 0) {
        printf(YELLOW "Operação cancelada.\n" RESET);
        return;
    }

    Evento *evento_selecionado = NULL;
    for (int i = 0; i < count; i++) {
        if (opcoes[i] == escolha) {
            evento_selecionado = eventos_inscrito[i];
            break;
        }
    }

    if (!evento_selecionado) {
        printf(RED "Opção inválida.\n" RESET);
        return;
    }

    if (efetuarRemocaoInscricao(evento_selecionado, email_usuario)) {
        printf(GREEN "\nInscrição removida com sucesso!\n" RESET);
    } else {
        printf(RED "\nFalha ao remover inscrição.\n" RESET);
    }
}

void adminRemoverInscricaoEspecifica() {
    if (!eventos) {
        printf(YELLOW "\nNenhum evento cadastrado.\n" RESET);
        return;
    }

    printf(BLUE "\n=== REMOVER INSCRIÇÃO (ADMIN) ===\n" RESET);
    listarEventos();

    char nome_evento[MAX];
    printf("\nNome do evento: ");
    obterStringSegura(nome_evento, MAX, "");

    Evento *evento = buscaIndexada(nome_evento);
    if (!evento) {
        evento = eventos;
        while (evento && strcmp(evento->nome, nome_evento) != 0) {
            evento = evento->prox;
        }
    }

    if (!evento) {
        printf(RED "Evento não encontrado.\n" RESET);
        return;
    }

    printf(BLUE "\nParticipantes inscritos:\n" RESET);
    if (!evento->participantes) {
        printf(YELLOW "Nenhum participante inscrito.\n" RESET);
        return;
    }

    Part *p_atual = evento->participantes->prox;
    Part *inicio_iter = evento->participantes->prox;
    do {
        printf(CYAN "- %s (%s)\n" RESET, p_atual->nome, p_atual->email);
        p_atual = p_atual->prox;
    } while (p_atual != inicio_iter);


    char email[MAX];
    printf("\nEmail do participante a remover: ");
    obterStringSegura(email, MAX, "");

    if (!validarEmail(email)) {
        printf(RED "Email inválido.\n" RESET);
        return;
    }

    if (efetuarRemocaoInscricao(evento, email)) {
        printf(GREEN "Inscrição removida com sucesso.\n" RESET);
    } else {
        printf(RED "Falha ao remover inscrição.\n" RESET);
    }
}

void listarTodasInscricoes() {
    if (!todas_inscricoes) {
        printf(YELLOW "\nNenhuma inscrição no sistema.\n" RESET);
        return;
    }

    printf(MAGENTA "\n=== TODAS AS INSCRIÇÕES ===\n" RESET);
    Inscricao *atual = todas_inscricoes;
    while (atual != NULL) {
        Evento *e = eventos;
        while (e && e->id != atual->evento_id)
            e = e->prox;

        if (e) {
            printf(CYAN "Evento: %-20s | Participante: %-20s (%s)\n" RESET,
                   e->nome, atual->nome_participante, atual->email_participante);
        } else {
            printf(RED "Evento ID:%d (não encontrado) | Participante: %s\n" RESET,
                   atual->evento_id, atual->nome_participante);
        }
        atual = atual->prox;
    }
}

void listarMinhasInscricoes(const char *email_usuario) {
    if (!email_usuario || !todas_inscricoes) {
        printf(YELLOW "\nNenhuma inscrição encontrada.\n" RESET);
        return;
    }

    printf(BLUE "\n=== MINHAS INSCRIÇÕES ===\n" RESET);
    int count = 0;
    Inscricao *atual = todas_inscricoes;

    while (atual != NULL) {
        if (strcmp(atual->email_participante, email_usuario) == 0) {
            Evento *e = eventos;
            while (e && e->id != atual->evento_id)
                e = e->prox;

            if (e) {
                printf(CYAN "%d. %s (%s) - Vagas: %d/%d\n" RESET,
                       ++count, e->nome, e->categoria,
                       e->max_vagas - e->vagas, e->max_vagas);
            }
        }
        atual = atual->prox;
    }

    if (count == 0) {
        printf(YELLOW "Você não possui inscrições ativas.\n" RESET);
    }
}
