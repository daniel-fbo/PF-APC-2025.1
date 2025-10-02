
#ifndef INTERFACE_H
#define INTERFACE_H

#include "database.h"
#include "validacao.h"
#include <gtk/gtk.h>

// Estrutura para conectar a interconectar a interface
typedef struct {
    MYSQL *conn;
    GtkWidget *window;
    GtkWidget *stack;
    
    // Widgets da tela de cadastro
    GtkWidget *entry_cpf;
    GtkWidget *entry_nome;
    GtkWidget *combo_cargo;
    GtkWidget *combo_tempo;
    GtkWidget *entry_salario;
    GtkWidget *switch_clausula;
    GtkWidget *spin_dependentes;
    GtkWidget *combo_plano;
    // Adicionar no struct WIDGETS:


    
    // Widgets da tela de busca
    GtkWidget *entry_busca_cpf;
    GtkWidget *label_resultado;
} WIDGETS;

// ===================== FUNÇÕES AUXILIARES =====================

void mostrar_mensagem(GtkWindow *parent, const char *titulo, const char *mensagem, GtkMessageType tipo);
void limpar_formulario(GtkWidget *container);

// ===================== CALLBACKS =====================

static void on_ir_para_busca(GtkButton *button, gpointer user_data);
static void on_voltar_principal(GtkButton *button, gpointer user_data);
static void on_salvar_candidatura(GtkButton *button, gpointer user_data);
static void on_buscar_candidatura(GtkButton *button, gpointer user_data);
static void on_administrar(GtkButton *button, gpointer user_data);
static void on_nova_candidatura(GtkButton *button, gpointer user_data);

// ===================== CONSTRUÇÃO DE TELAS =====================

void construir_tela_principal(WIDGETS *data);
void construir_tela_cadastro(WIDGETS *data);
void construir_tela_busca(WIDGETS *data);

// ===================== FUNÇÃO PRINCIPAL =====================

void iniciar_interface(int *argc, char ***argv);

#endif
