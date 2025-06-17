#include "Participantes/participantes.h"
#include "Utils/utils.h"

void inicializarHashParticipantes() {
    for (int i = 0; i < HASH_SIZE; i++) {
        hash_table[i] = NULL;
    }
}

int hashParticipante(const char *email) {
    if (!email) return 0;
    
    unsigned long hash = 5381;
    int c;
    while ((c = *email++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_SIZE;
}

Part* buscaParticipantePorEmailNaHash(const char *email) {
    if (!email || strlen(email) == 0) return NULL;
    
    char email_normalizado[MAX];
    strncpy(email_normalizado, email, MAX - 1);
    email_normalizado[MAX - 1] = '\0';
    
    for (int i = 0; email_normalizado[i]; i++) {
        email_normalizado[i] = tolower(email_normalizado[i]);
    }
    
    int index = hashParticipante(email_normalizado);
    Part *p = hash_table[index];
    
    while (p != NULL) {
        if (strcmp(p->email, email_normalizado) == 0) {
            return p;
        }
        p = p->prox;
    }
    return NULL;
}

Part* obterOuCriarParticipante(const char *email_usuario) {
    if (!email_usuario || strlen(email_usuario) == 0) {
        printf(RED "Email inválido: string vazia!\n" RESET);
        return NULL;
    }
    
    if (strlen(email_usuario) >= MAX) {
        printf(RED "Email inválido: muito longo (máx %d caracteres)!\n" RESET, MAX - 1);
        return NULL;
    }
    
    if (!validarEmail(email_usuario)) {
        printf(RED "Email inválido: formato incorreto!\n" RESET);
        return NULL;
    }

    Part *p = buscaParticipantePorEmailNaHash(email_usuario);
    if (p != NULL) {
        printf(YELLOW "Usando cadastro existente para %s (ID: %d)\n" RESET, p->nome, p->id);
        return p;
    }

    p = malloc(sizeof(Part));
    if (!p) {
        printf(RED "Erro de alocação para novo participante!\n" RESET);
        return NULL;
    }
    
    p->id = next_part_id++;
    
    strncpy(p->email, email_usuario, MAX - 1);
    p->email[MAX - 1] = '\0';
    for (int i = 0; p->email[i]; i++) {
        p->email[i] = tolower(p->email[i]);
    }
    
    char nome_buffer[MAX];
    printf("Digite seu nome (máx %d caracteres): ", MAX - 1);
    obterStringSegura(nome_buffer, MAX, "");
    
    if (strlen(nome_buffer) == 0) {
        printf(RED "Nome não pode ser vazio!\n" RESET);
        free(p);
        return NULL;
    }
    
    if (!contemApenasLetras(nome_buffer)) {
        printf(RED "Nome inválido: deve conter apenas letras e espaços!\n" RESET);
        free(p);
        return NULL;
    }
    strcpy(p->nome, nome_buffer);
    
    int index = hashParticipante(p->email);
    p->prox = hash_table[index];
    hash_table[index] = p;
    
    printf(GREEN "Novo participante criado com sucesso! (ID: %d)\n" RESET, p->id);
    return p;
}
