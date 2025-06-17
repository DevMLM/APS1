#include "Utils/utils.h"

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

bool validarEmail(const char *email) {
    if (!email || strlen(email) == 0) {
        return false;
    }

    if (strlen(email) >= MAX) {
        return false;
    }

    char *ptr_arroba = strchr(email, '@');
    if (!ptr_arroba || ptr_arroba == email) {
        return false;
    }

    char *ptr_ponto = strchr(ptr_arroba, '.');
    if (!ptr_ponto || ptr_ponto == ptr_arroba + 1) {
        return false;
    }

    if (strlen(ptr_ponto) < 2) {
        return false;
    }
    
    int count_arroba = 0;
    for (int i = 0; email[i]; i++) {
        if (email[i] == '@') {
            count_arroba++;
        }
    }
    if (count_arroba != 1) return false;

    return true;
}

bool autenticarAdmin(const char *senha) {
    if (!senha) return false;
    return strcmp(senha, ADMIN_PASS) == 0;
}

void lerSenha(char *senha, int tamanho_max) {
    int i = 0;
    char ch;
    
    #ifdef _WIN32
    while ((ch = _getch()) != '\r' && i < tamanho_max - 1) {
        if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (isprint(ch)) {
            senha[i++] = ch;
            printf("*");
        }
    }
    #else
    printf(" (ATENCAO: Senha visivel em sistemas Unix-like sem termios.h)\n");
    while ((ch = getchar()) != '\n' && i < tamanho_max - 1) {
        if (ch == 127 || ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (isprint(ch)) {
            senha[i++] = ch;
            printf("*");
        }
    }
    #endif
    
    senha[i] = '\0';
    printf("\n");
}

void capitalizarNome(char *nome) {
    if (!nome || strlen(nome) == 0) return;

    nome[0] = toupper(nome[0]);

    for (int i = 1; nome[i]; i++) {
        if (nome[i-1] == ' ') {
            nome[i] = toupper(nome[i]);
        } else {
            nome[i] = tolower(nome[i]);
        }
    }
}

void removerEspacosExtras(char *str) {
    if (!str) return;
    
    int i, j;
    for (i = 0; str[i] && isspace(str[i]); i++);
    
    for (j = 0; str[i]; i++) {
        if (!isspace(str[i]) || (j > 0 && !isspace(str[j-1]))) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
    
    if (j > 0 && isspace(str[j-1])) {
        str[j-1] = '\0';
    }
}

bool contemApenasLetras(const char *str) {
    if (!str) return false;
    
    for (int i = 0; str[i]; i++) {
        if (!isalpha(str[i]) && !isspace(str[i])) {
            return false;
        }
    }
    return true;
}

void obterStringSegura(char *buffer, int tamanho, const char *mensagem) {
    if (!buffer || tamanho <= 0) return;
    
    printf("%s", mensagem);
    if (fgets(buffer, tamanho, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    
    buffer[strcspn(buffer, "\n")] = '\0';
    
    if (strlen(buffer) == tamanho - 1) {
        limparBuffer();
    }
    
    removerEspacosExtras(buffer);
    capitalizarNome(buffer);
}
