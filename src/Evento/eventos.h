#ifndef EVENTOS_H
#define EVENTOS_H

#include "comum.h"

void cadastrarEvento();
void listarEventos();
void bubbleSort();
Evento* merge(Evento *a, Evento *b);
void split(Evento *source, Evento **front, Evento **back);
void mergeSort(Evento **head);
void atualizarIndice();
Evento* buscaIndexada(char *nome);

#endif // EVENTOS_H
