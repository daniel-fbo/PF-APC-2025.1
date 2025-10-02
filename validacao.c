#include "validacao.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

bool validar_cpf(const char *cpf) {
    if (cpf == NULL || strlen(cpf) != 11) return false;
    if (strlen(cpf) != 11) return false;
    
    // Verifica se todos os caracteres são dígitos
    for (int i = 0; i < 11; i++) {
        if (!isdigit((unsigned char) cpf[i])) return false;
    }

    return true;
}

bool validar_nome(const char *nome) {
    if (nome == NULL || strlen(nome) < 3 || strlen(nome) > 50) return false;
    
    // Verifica se contém apenas letras e espaços
    for (size_t i = 0; i < strlen(nome); i++) {
        if (!isalpha(nome[i]) && nome[i] != ' ') return false;
    }
    
    return true;
}

bool validar_tempo_contrato(int tempo) {
    return tempo == 12 || tempo == 18 || tempo == 24;
}

bool validar_dependentes(int dependentes) {
    return dependentes >= 0 && dependentes <= 10;
}

bool validar_plano_saude(const char *plano) {
    return strcmp(plano, "Básico") == 0 || 
           strcmp(plano, "Premium") == 0 ||
           strcmp(plano, "Não contratado") == 0;
}

bool validar_opcao(int opcao, int min, int max) {
    return opcao >= min && opcao <= max;
}
