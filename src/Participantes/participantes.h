#ifndef PARTICIPANTES_H
#define PARTICIPANTES_H

#include "comum.h"

void inicializarHashParticipantes();

int hashParticipante(const char *email);
Part* buscaParticipantePorEmailNaHash(const char *email);
Part* obterOuCriarParticipante(const char *email_usuario);

#endif // PARTICIPANTES_H
