
#ifndef DATABASE_H
#define DATABASE_H

#include <mysql.h>
#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct {
    char cpf[12];
    char nome[50];
    char cargo[20];
    int tempo_contrato;
    float salario;
    float clausula;
    int dependentes;
    char plano_saude[20];
} Candidatura;

bool conectar_banco(MYSQL *conn);
void desconectar_banco(MYSQL *conn);
bool inserir_candidatura(MYSQL *conn, Candidatura *cand);
bool buscar_candidatura(MYSQL *conn, const char *cpf, Candidatura *cand);
GList* listar_candidaturas(MYSQL *conn);

#endif

