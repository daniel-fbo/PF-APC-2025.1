
#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool conectar_banco(MYSQL *conn) {
    const char *host = "localhost";
    const char *user = "diac_user";
    const char *pass = "diac123";
    const char *db = "diac_db";
    unsigned int port = 3306;

    if (!mysql_real_connect(conn, host, user, pass, db, port, NULL, 0)) {
        fprintf(stderr, "Erro na conexão MySQL: %s\n", mysql_error(conn));
        return false;
    }
    return true;
}

void desconectar_banco(MYSQL *conn) {
    mysql_close(conn);
}

bool inserir_candidatura(MYSQL *conn, Candidatura *cand) {
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "Erro ao inicializar statement\n");
        return false;
    }

    const char *query = "INSERT INTO candidaturas (cpf, nome, cargo, tempo_contrato, salario, "
                       "clausula_rescisao, dependentes, plano_saude) VALUES (?, ?, ?, ?, ?, ?, ?, ?) "
                       "ON DUPLICATE KEY UPDATE nome=?, cargo=?, tempo_contrato=?, salario=?, "
                       "clausula_rescisao=?, dependentes=?, plano_saude=?";

    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "Erro ao preparar query: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[15];
    memset(bind, 0, sizeof(bind));

    // Configuração dos parâmetros de entrada
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = cand->cpf;
    bind[0].buffer_length = strlen(cand->cpf);

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = cand->nome;
    bind[1].buffer_length = strlen(cand->nome);

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = cand->cargo;
    bind[2].buffer_length = strlen(cand->cargo);

    bind[3].buffer_type = MYSQL_TYPE_LONG;
    bind[3].buffer = &(cand->tempo_contrato);

    bind[4].buffer_type = MYSQL_TYPE_FLOAT;
    bind[4].buffer = &(cand->salario);

    bind[5].buffer_type = MYSQL_TYPE_FLOAT;
    bind[5].buffer = &(cand->clausula);

    bind[6].buffer_type = MYSQL_TYPE_LONG;
    bind[6].buffer = &(cand->dependentes);

    bind[7].buffer_type = MYSQL_TYPE_STRING;
    bind[7].buffer = cand->plano_saude;
    bind[7].buffer_length = strlen(cand->plano_saude);

    // Configuração dos parâmetros para atualização (ON DUPLICATE KEY UPDATE)
    bind[8].buffer_type = MYSQL_TYPE_STRING;
    bind[8].buffer = cand->nome;
    bind[8].buffer_length = strlen(cand->nome);

    bind[9].buffer_type = MYSQL_TYPE_STRING;
    bind[9].buffer = cand->cargo;
    bind[9].buffer_length = strlen(cand->cargo);

    bind[10].buffer_type = MYSQL_TYPE_LONG;
    bind[10].buffer = &(cand->tempo_contrato);

    bind[11].buffer_type = MYSQL_TYPE_FLOAT;
    bind[11].buffer = &(cand->salario);

    bind[12].buffer_type = MYSQL_TYPE_FLOAT;
    bind[12].buffer = &(cand->clausula);

    bind[13].buffer_type = MYSQL_TYPE_LONG;
    bind[13].buffer = &(cand->dependentes);

    bind[14].buffer_type = MYSQL_TYPE_STRING;
    bind[14].buffer = cand->plano_saude;
    bind[14].buffer_length = strlen(cand->plano_saude);

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "Erro ao vincular parâmetros: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "Erro ao executar query: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_close(stmt);
    return true;
}

bool buscar_candidatura(MYSQL *conn, const char *cpf, Candidatura *cand) {
    // Inicializa a estrutura de resultado
    memset(cand, 0, sizeof(Candidatura));
    strncpy(cand->cpf, cpf, sizeof(cand->cpf)-1);

    // Validação básica do CPF
    if(!cpf || strlen(cpf) != 11) {
        fprintf(stderr, "CPF inválido ou não fornecido\n");
        return false;
    }
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "Erro ao inicializar statement\n");
        return false;
    }

    const char *query = "SELECT nome, cargo, tempo_contrato, salario, clausula_rescisao, "
                       "dependentes, plano_saude FROM candidaturas WHERE cpf = ?";

    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "Erro ao preparar query: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND param;
    memset(&param, 0, sizeof(param));
    param.buffer_type = MYSQL_TYPE_STRING;
    param.buffer = (char *)cpf;
    param.buffer_length = strlen(cpf);

    if (mysql_stmt_bind_param(stmt, &param)) {
        fprintf(stderr, "Erro ao vincular parâmetro: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    // Configuração dos resultados
    MYSQL_BIND result[7];
    memset(result, 0, sizeof(result));

    // Bufferes temporários para strings
    char nome_temp[50] = {0};
    char cargo_temp[20] = {0};
    char plano_temp[20] = {0};

    result[0].buffer_type = MYSQL_TYPE_STRING;
    result[0].buffer = nome_temp;
    result[0].buffer_length = sizeof(nome_temp);
    result[0].length = 0;
    result[0].is_null = 0;

    result[1].buffer_type = MYSQL_TYPE_STRING;
    result[1].buffer = cargo_temp;
    result[1].buffer_length = sizeof(cargo_temp);
    result[1].length = 0;
    result[1].is_null = 0;

    result[2].buffer_type = MYSQL_TYPE_LONG;
    result[2].buffer = &(cand->tempo_contrato);
    result[2].is_null = 0;

    result[3].buffer_type = MYSQL_TYPE_FLOAT;
    result[3].buffer = &(cand->salario);
    result[3].is_null = 0;

    result[4].buffer_type = MYSQL_TYPE_FLOAT;
    result[4].buffer = &(cand->clausula);
    result[4].is_null = 0;

    result[5].buffer_type = MYSQL_TYPE_LONG;
    result[5].buffer = &(cand->dependentes);
    result[5].is_null = 0;

    result[6].buffer_type = MYSQL_TYPE_STRING;
    result[6].buffer = plano_temp;
    result[6].buffer_length = sizeof(plano_temp);
    result[6].length = 0;
    result[6].is_null = 0;

    if (mysql_stmt_bind_result(stmt, result)) {
        fprintf(stderr, "Erro ao vincular resultados: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "Erro ao executar query: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    int fetch_result = mysql_stmt_fetch(stmt);
    
    if (fetch_result == MYSQL_NO_DATA) {
        fprintf(stderr, "Nenhum registro encontrado para CPF: %s\n", cpf);
        mysql_stmt_close(stmt);
        return false;
    } else if (fetch_result != 0) {
        fprintf(stderr, "Erro ao buscar dados: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    // Copia os dados dos buffers temporários para a estrutura
    strncpy(cand->nome, nome_temp, sizeof(cand->nome)-1);
    strncpy(cand->cargo, cargo_temp, sizeof(cand->cargo)-1);
    strncpy(cand->plano_saude, plano_temp, sizeof(cand->plano_saude)-1);

    mysql_stmt_close(stmt);
    return true;
}

GList* listar_candidaturas(MYSQL *conn) {
    GList *lista = NULL;
    
    if (mysql_query(conn, "SELECT cpf, nome, cargo, tempo_contrato, salario, clausula_rescisao, dependentes, plano_saude FROM candidaturas ORDER BY nome")) {
        fprintf(stderr, "Erro ao buscar candidaturas: %s\n", mysql_error(conn));
        return NULL;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (!result) {
        fprintf(stderr, "Erro ao obter resultados\n");
        return NULL;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        Candidatura *cand = g_new0(Candidatura, 1);
        
        strncpy(cand->cpf, row[0], sizeof(cand->cpf)-1);
        strncpy(cand->nome, row[1], sizeof(cand->nome)-1);
        strncpy(cand->cargo, row[2], sizeof(cand->cargo)-1);
        cand->tempo_contrato = atoi(row[3]);
        cand->salario = atof(row[4]);
        cand->clausula = atof(row[5]);
        cand->dependentes = atoi(row[6]);
        strncpy(cand->plano_saude, row[7], sizeof(cand->plano_saude)-1);
        
        lista = g_list_append(lista, cand);
    }

    mysql_free_result(result);
    return lista;
}

bool remover_candidatura(MYSQL *conn, const char *cpf) {
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "Erro ao inicializar statement\n");
        return false;
    }

    const char *query = "DELETE FROM candidaturas WHERE cpf = ?";

    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "Erro ao preparar query: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind;
    memset(&bind, 0, sizeof(bind));
    bind.buffer_type = MYSQL_TYPE_STRING;
    bind.buffer = (char *)cpf;
    bind.buffer_length = strlen(cpf);

    if (mysql_stmt_bind_param(stmt, &bind)) {
        fprintf(stderr, "Erro ao vincular parâmetro: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "Erro ao executar query: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_close(stmt);
    return true;
}
