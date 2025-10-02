#ifndef VALIDATION_H
#define VALIDATION_H
#define SENHA_ADMIN "senha123"

#include "database.h"
#include "stdbool.h"
#include <ctype.h>
#include <string.h>

bool validar_cpf(const char *cpf);
bool validar_nome(const char *nome);
bool validar_opcao(int opcao, int min, int max);
bool validar_tempo_contrato(int tempo);
bool validar_dependentes(int dependentes) ;
bool validar_plano_saude(const char *plano) ;

#endif
