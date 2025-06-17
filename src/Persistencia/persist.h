#ifndef PERSIST_H
#define PERSIST_H

#include "comum.h"
#include "Utils/utils.h"
#include "Participantes/participantes.h"
#include "Inscricoes/inscricoes.h"
#include "Evento/eventos.h"

void salvarEventosCSV();
void salvarParticipantesCSV();
void salvarInscricoesCSV();
void salvarFilaEsperaCSV();
void salvarTudoCSV();

void carregarEventosCSV();
void carregarParticipantesCSV();
void carregarInscricoesCSV();
void carregarFilaEsperaCSV();
void carregarTudoCSV();

void liberarMemoria();

#endif // PERSIST_H
