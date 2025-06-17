#ifndef INSCRICOES_H
#define INSCRICOES_H

#include "comum.h"
#include "Utils/utils.h"
#include "Participantes/participantes.h"
#include "FilaPilha/filaPilha.h"

void insereParticipante(Evento *e, Part *p);
int buscaParticipanteEmEvento(Part *lista_participantes_evento, const char *email); // Adicionado 'const'
int efetuarRemocaoInscricao(Evento *evento_alvo, const char *email_participante);
void removerInscricaoUsuario(const char *email_usuario); // Adicionado 'const'
void adminRemoverInscricaoEspecifica();
void listarTodasInscricoes();
void listarMinhasInscricoes(const char *email_usuario); // Novo protótipo

#endif // INSCRICOES_H
