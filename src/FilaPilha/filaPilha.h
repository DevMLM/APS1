#ifndef FILA_PILHA_H
#define FILA_PILHA_H

#include "comum.h"

void insereFilaEspera(Evento *e, Part *p);
void listarFilaEspera();

void pushEvento(Evento *e);
Evento* popEvento();
void listarPilhaEventos();

#endif // FILA_PILHA_H
