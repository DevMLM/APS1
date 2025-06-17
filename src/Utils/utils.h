#ifndef UTILS_H
#define UTILS_H

#include "comum.h"

void limparBuffer();
bool validarEmail(const char *email);
bool autenticarAdmin(const char *senha);
void lerSenha(char *buffer, int max_len);

void capitalizarNome(char *nome);
void removerEspacosExtras(char *str);
bool contemApenasLetras(const char *str);
void obterStringSegura(char *buffer, int tamanho, const char *mensagem);

#endif // UTILS_H
