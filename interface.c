#include "interface.h"
#include "validacao.h"
#include <gtk/gtk.h>
#include <string.h>

// ===================== FUNÇÕES AUXILIARES =====================

void mostrar_mensagem(GtkWindow *parent, const char *titulo, const char *mensagem, GtkMessageType tipo) {
    GtkWidget *dialog = gtk_message_dialog_new(parent,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        tipo,
        GTK_BUTTONS_OK,
        "%s", mensagem);
    
    gtk_window_set_title(GTK_WINDOW(dialog), titulo);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void limpar_formulario(GtkWidget *container) {
    GList *children, *iter;
    
    children = gtk_container_get_children(GTK_CONTAINER(container));
    for(iter = children; iter != NULL; iter = g_list_next(iter)) {
        if(GTK_IS_ENTRY(iter->data)) {
            gtk_entry_set_text(GTK_ENTRY(iter->data), "");
        }
        else if(GTK_IS_COMBO_BOX(iter->data)) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(iter->data), 0);
        }
        else if(GTK_IS_SPIN_BUTTON(iter->data)) {
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(iter->data), 0);
        }
    }
    g_list_free(children);
}

// ===================== CALLBACKS =====================
static void on_ir_para_busca(GtkButton *button, gpointer user_data) {
    WIDGETS *data = (WIDGETS *)user_data;
    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "busca");
}

static void on_voltar_principal(GtkButton *button, gpointer user_data) {
    WIDGETS *data = (WIDGETS *)user_data;
    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "principal");
}

static void on_salvar_candidatura(GtkButton *button, gpointer user_data) {
    WIDGETS *data = (WIDGETS *)user_data;
    Candidatura cand;
    
    // Obter valores do formulário
    const char *cpf = gtk_entry_get_text(GTK_ENTRY(data->entry_cpf));
    const char *nome = gtk_entry_get_text(GTK_ENTRY(data->entry_nome));
    
    // Validações
    if(!validar_cpf(cpf)) {
        mostrar_mensagem(GTK_WINDOW(data->window), "Erro", "CPF inválido!", GTK_MESSAGE_ERROR);
        return;
    }
    
    if(!validar_nome(nome)) {
        mostrar_mensagem(GTK_WINDOW(data->window), "Erro", "Nome inválido!", GTK_MESSAGE_ERROR);
        return;
    }
    
    // Preencher estrutura Candidatura
    strncpy(cand.cpf, cpf, sizeof(cand.cpf));
    strncpy(cand.nome, nome, sizeof(cand.nome));
    
    // Obter cargo selecionado
    gint cargo_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(data->combo_cargo));
    const char *cargos[] = {"Gerente", "Técnico", "Analista"};
    strncpy(cand.cargo, cargos[cargo_idx], sizeof(cand.cargo));
    
    // Obter tempo de contrato
    gint tempo_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(data->combo_tempo));
    int tempos[] = {12, 18, 24};
    cand.tempo_contrato = tempos[tempo_idx];
    
    // Obter salário
    cand.salario = atof(gtk_entry_get_text(GTK_ENTRY(data->entry_salario)));

// Clausula de rescisão
    cand.clausula = gtk_switch_get_active(GTK_SWITCH(data->switch_clausula)) ? 5000.0 : 0.0;

    
    // Dependentes
    cand.dependentes = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(data->spin_dependentes));
    
    // Plano de saúde
    gint plano_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(data->combo_plano));
    const char *planos[] = {"Básico", "Premium", "Não contratado"};
    strncpy(cand.plano_saude, planos[plano_idx], sizeof(cand.plano_saude));
    
    // Inserir no banco de dados
    if(inserir_candidatura(data->conn, &cand)) {
        mostrar_mensagem(GTK_WINDOW(data->window), "Sucesso", "Candidatura salva com sucesso!", GTK_MESSAGE_INFO);
        limpar_formulario(gtk_widget_get_parent(data->entry_cpf));
        gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "principal");
    } else {
        mostrar_mensagem(GTK_WINDOW(data->window), "Erro", "Falha ao salvar candidatura!", GTK_MESSAGE_ERROR);
    }
}

static void on_buscar_candidatura(GtkButton *button, gpointer user_data) {
    WIDGETS *data = (WIDGETS *)user_data;

    printf(">>> on_buscar_candidatura foi chamado\n");

    const char *cpf = gtk_entry_get_text(GTK_ENTRY(data->entry_busca_cpf));
    if (!cpf) {
        mostrar_mensagem(GTK_WINDOW(data->window), "Erro", "Campo CPF está nulo!", GTK_MESSAGE_ERROR);
        return;
    }

    printf(">>> CPF lido: '%s' (len=%zu)\n", cpf, strlen(cpf));

    if (strlen(cpf) == 0) {
        mostrar_mensagem(GTK_WINDOW(data->window), "Erro", "Digite um CPF antes de buscar!", GTK_MESSAGE_WARNING);
        return;
    }

    if (!validar_cpf(cpf)) {
        mostrar_mensagem(GTK_WINDOW(data->window), "Erro", "CPF inválido!", GTK_MESSAGE_ERROR);
        return;
    }

    Candidatura cand;
    if (buscar_candidatura(data->conn, cpf, &cand)) {
        char resultado[512];
        snprintf(resultado, sizeof(resultado),
            "Nome: %s\n"
            "CPF: %s\n"
            "Cargo: %s\n"
            "Tempo de Contrato: %d meses\n"
            "Salário: R$ %.2f\n"
            "Cláusula: R$ %.2f\n"
            "Dependentes: %d\n"
            "Plano de Saúde: %s",
            cand.nome, cand.cpf, cand.cargo, cand.tempo_contrato,
            cand.salario, cand.clausula, cand.dependentes, cand.plano_saude);
        
        gtk_label_set_text(GTK_LABEL(data->label_resultado), resultado);
    } else {
        gtk_label_set_text(GTK_LABEL(data->label_resultado), "Nenhuma candidatura encontrada para este CPF");
    }
}

static void on_administrar(GtkButton *button, gpointer user_data) {
    WIDGETS *data = (WIDGETS *)user_data;
    
    // Criar diálogo de senha
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Acesso Administrativo",
        GTK_WINDOW(data->window),
        GTK_DIALOG_MODAL,
        "Cancelar", GTK_RESPONSE_CANCEL,
        "Acessar", GTK_RESPONSE_ACCEPT,
        NULL);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry_senha = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entry_senha), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_senha), "Digite a senha de administrador");
    
    gtk_container_add(GTK_CONTAINER(content), entry_senha);
    gtk_widget_show_all(dialog);
    
    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *senha = gtk_entry_get_text(GTK_ENTRY(entry_senha));
        
        if(strcmp(senha, SENHA_ADMIN) == 0) {
            // Carregar lista de candidaturas
            GList *candidaturas = listar_candidaturas(data->conn);
            
            // Criar janela de administração
            GtkWidget *admin_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
            gtk_window_set_title(GTK_WINDOW(admin_window), "Administração - Candidaturas");
            gtk_window_set_default_size(GTK_WINDOW(admin_window), 800, 600);
            
            GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
            GtkWidget *treeview = gtk_tree_view_new();
            GtkListStore *store = gtk_list_store_new(8, 
                G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
                G_TYPE_INT, G_TYPE_FLOAT, G_TYPE_FLOAT,
                G_TYPE_INT, G_TYPE_STRING);
                
            // Configurar colunas
            GtkCellRenderer *renderer;
            GtkTreeViewColumn *column;
            
            const char *colunas[] = {"CPF", "Nome", "Cargo", "Tempo (meses)", "Salário", "Cláusula", "Dependentes", "Plano Saúde"};
            
            for(int i = 0; i < 8; i++) {
                renderer = gtk_cell_renderer_text_new();
                column = gtk_tree_view_column_new_with_attributes(colunas[i], renderer, "text", i, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
            }
            
            // Popular a lista
            GList *iter;
            for(iter = candidaturas; iter != NULL; iter = g_list_next(iter)) {
                Candidatura *c = (Candidatura *)iter->data;
                GtkTreeIter tree_iter;
                
                gtk_list_store_append(store, &tree_iter);
                gtk_list_store_set(store, &tree_iter,
                    0, c->cpf, 1, c->nome, 2, c->cargo,
                    3, c->tempo_contrato, 4, c->salario,
                    5, c->clausula, 6, c->dependentes,
                    7, c->plano_saude, -1);
            }
            
            gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
            g_object_unref(store);
            
            gtk_container_add(GTK_CONTAINER(scrolled), treeview);
            gtk_container_add(GTK_CONTAINER(admin_window), scrolled);
            gtk_widget_show_all(admin_window);
        } else {
            mostrar_mensagem(GTK_WINDOW(data->window), "Acesso Negado", "Senha incorreta!", GTK_MESSAGE_ERROR);
        }
    }
    
    gtk_widget_destroy(dialog);
}

static void on_nova_candidatura(GtkButton *button, gpointer user_data) {
    WIDGETS *data = (WIDGETS *)user_data;
    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "cadastro");
}



// ===================== CONSTRUÇÃO DE TELAS =====================

void construir_tela_principal(WIDGETS *data) {
    // Cria o box principal
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(box), 20);
    
    // Título
    GtkWidget *title = gtk_label_new("Sistema de Candidaturas DIAC");
    PangoAttrList *attrs = pango_attr_list_new();
    pango_attr_list_insert(attrs, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    pango_attr_list_insert(attrs, pango_attr_scale_new(1.5));
    gtk_label_set_attributes(GTK_LABEL(title), attrs);
    pango_attr_list_unref(attrs);
    gtk_widget_set_halign(title, GTK_ALIGN_CENTER);
    
    // Botões
    GtkWidget *btn_novo = gtk_button_new_with_label("Nova Candidatura");
    GtkWidget *btn_buscar = gtk_button_new_with_label("Buscar Candidatura");
    GtkWidget *btn_admin = gtk_button_new_with_label("Acesso Administrativo");
    
    // Conectar sinais
    g_signal_connect(btn_novo, "clicked", G_CALLBACK(on_nova_candidatura), data);
    g_signal_connect(btn_buscar, "clicked", G_CALLBACK(on_ir_para_busca), data);
    g_signal_connect(btn_admin, "clicked", G_CALLBACK(on_administrar), data);
    
    // Layout
    gtk_box_pack_start(GTK_BOX(box), title, FALSE, FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box), btn_novo, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(box), btn_buscar, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(box), btn_admin, FALSE, FALSE, 10);
    
    // Adicionar ao stack
    gtk_stack_add_named(GTK_STACK(data->stack), box, "principal");
}

void construir_tela_cadastro(WIDGETS *data) {
    // Cria a stack principal
    GtkWidget *box_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(box_principal, TRUE);
    gtk_widget_set_vexpand(box_principal, TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(box_principal), 20);

    // Título
    GtkWidget *lbl_titulo = gtk_label_new("CADASTRO");
    PangoAttrList *attrs = pango_attr_list_new();
    pango_attr_list_insert(attrs, pango_attr_scale_new(1.5));
    gtk_label_set_attributes(GTK_LABEL(lbl_titulo), attrs);
    gtk_label_set_xalign(GTK_LABEL(lbl_titulo), 0.5);
    gtk_box_pack_start(GTK_BOX(box_principal), lbl_titulo, FALSE, FALSE, 20);

    // Grid para os campos da tabela
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 15);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 15);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    // CPF
    GtkWidget *lbl_cpf = gtk_label_new("CPF:");
    gtk_widget_set_halign(lbl_cpf, GTK_ALIGN_END);
    data->entry_cpf = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->entry_cpf), "Somente números");
    gtk_widget_set_hexpand(data->entry_cpf, TRUE);
    gtk_grid_attach(GTK_GRID(grid), lbl_cpf, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), data->entry_cpf, 1, 0, 2, 1);

    // Nome
    GtkWidget *lbl_nome = gtk_label_new("Nome Completo:");
    gtk_widget_set_halign(lbl_nome, GTK_ALIGN_END);
    data->entry_nome = gtk_entry_new();
    gtk_widget_set_hexpand(data->entry_nome, TRUE);
    gtk_grid_attach(GTK_GRID(grid), lbl_nome, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), data->entry_nome, 1, 1, 2, 1);

    // Cargo
    GtkWidget *lbl_cargo = gtk_label_new("Cargo:");
    gtk_widget_set_halign(lbl_cargo, GTK_ALIGN_END);
    data->combo_cargo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->combo_cargo), "Gerente");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->combo_cargo), "Técnico");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->combo_cargo), "Analista");
    gtk_combo_box_set_active(GTK_COMBO_BOX(data->combo_cargo), 0);
    gtk_widget_set_hexpand(data->combo_cargo, TRUE);
    gtk_grid_attach(GTK_GRID(grid), lbl_cargo, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), data->combo_cargo, 1, 2, 2, 1);

    // Tempo de Contrato
    GtkWidget *lbl_tempo = gtk_label_new("Tempo de Contrato:");
    gtk_widget_set_halign(lbl_tempo, GTK_ALIGN_END);
    data->combo_tempo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->combo_tempo), "12 meses");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->combo_tempo), "18 meses");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->combo_tempo), "24 meses");
    gtk_combo_box_set_active(GTK_COMBO_BOX(data->combo_tempo), 0);
    gtk_widget_set_hexpand(data->combo_tempo, TRUE);
    gtk_grid_attach(GTK_GRID(grid), lbl_tempo, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), data->combo_tempo, 1, 3, 2, 1);

    // Salário
    GtkWidget *lbl_salario = gtk_label_new("Salário Pretendido:");
    gtk_widget_set_halign(lbl_salario, GTK_ALIGN_END);
    data->entry_salario = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->entry_salario), "R$");
    gtk_widget_set_hexpand(data->entry_salario, TRUE);
    gtk_grid_attach(GTK_GRID(grid), lbl_salario, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), data->entry_salario, 1, 4, 2, 1);

    // Linha switch e butao spin
    GtkWidget *box_linha = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_widget_set_hexpand(box_linha, TRUE);
    
    // Box para cláusula
    GtkWidget *box_clausula = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *lbl_clausula = gtk_label_new("Cláusula de Rescisão:");
    gtk_widget_set_halign(lbl_clausula, GTK_ALIGN_END);
    data->switch_clausula = gtk_switch_new();
    gtk_box_pack_start(GTK_BOX(box_clausula), lbl_clausula, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_clausula), data->switch_clausula, FALSE, FALSE, 0);
    
    // Box para dependentes
    GtkWidget *box_dependentes = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *lbl_dependentes = gtk_label_new("Dependentes:");
    gtk_widget_set_halign(lbl_dependentes, GTK_ALIGN_END);
    data->spin_dependentes = gtk_spin_button_new_with_range(0, 10, 1);
    gtk_box_pack_start(GTK_BOX(box_dependentes), lbl_dependentes, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_dependentes), data->spin_dependentes, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(box_linha), box_clausula, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_linha), box_dependentes, TRUE, TRUE, 0);
    gtk_grid_attach(GTK_GRID(grid), box_linha, 0, 5, 3, 1);

    // Plano de Saúde
    GtkWidget *lbl_plano = gtk_label_new("Plano de Saúde:");
    gtk_widget_set_halign(lbl_plano, GTK_ALIGN_END);
    data->combo_plano = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->combo_plano), "Básico");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->combo_plano), "Premium");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->combo_plano), "Não contratado");
    gtk_combo_box_set_active(GTK_COMBO_BOX(data->combo_plano), 0);
    gtk_widget_set_hexpand(data->combo_plano, TRUE);
    gtk_grid_attach(GTK_GRID(grid), lbl_plano, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), data->combo_plano, 1, 6, 2, 1);

    // Botões
    GtkWidget *box_botoes = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(box_botoes), GTK_BUTTONBOX_END);
    gtk_box_set_spacing(GTK_BOX(box_botoes), 10);
    
    GtkWidget *btn_voltar = gtk_button_new_with_label("Voltar");
    GtkWidget *btn_salvar = gtk_button_new_with_label("Salvar");
    gtk_widget_set_size_request(btn_voltar, 100, -1);
    gtk_widget_set_size_request(btn_salvar, 100, -1);
    g_signal_connect(btn_voltar, "clicked", G_CALLBACK(on_voltar_principal), data);
    g_signal_connect(btn_salvar, "clicked", G_CALLBACK(on_salvar_candidatura), data);
    
    gtk_container_add(GTK_CONTAINER(box_botoes), btn_voltar);
    gtk_container_add(GTK_CONTAINER(box_botoes), btn_salvar);
    gtk_grid_attach(GTK_GRID(grid), box_botoes, 0, 7, 3, 1);

    // Adicionar grid ao box principal
    gtk_box_pack_start(GTK_BOX(box_principal), grid, FALSE, FALSE, 0);
    
    // Adicionar ao stack
    gtk_stack_add_named(GTK_STACK(data->stack), box_principal, "cadastro");
}

void construir_tela_busca(WIDGETS *data) {
    // Cria o box principal
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(box), 20);
    
    // Campo de busca
    GtkWidget *lbl_busca = gtk_label_new("Digite o CPF para buscar:");
    data->entry_busca_cpf = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->entry_busca_cpf), "Somente números");
    
    GtkWidget *btn_buscar = gtk_button_new_with_label("Buscar");
    g_signal_connect(btn_buscar, "clicked", G_CALLBACK(on_buscar_candidatura), data);
    
    GtkWidget *btn_voltar = gtk_button_new_with_label("Voltar");
    g_signal_connect(btn_voltar, "clicked", G_CALLBACK(on_voltar_principal), data);
    
    // Área de resultados
    data->label_resultado = gtk_label_new("");
    gtk_label_set_line_wrap(GTK_LABEL(data->label_resultado), TRUE);
    gtk_label_set_selectable(GTK_LABEL(data->label_resultado), TRUE);
    
    // Layout
    gtk_box_pack_start(GTK_BOX(box), lbl_busca, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), data->entry_busca_cpf, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), btn_buscar, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(box), data->label_resultado, TRUE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(box), btn_voltar, FALSE, FALSE, 5);
    
    gtk_stack_add_named(GTK_STACK(data->stack), box, "busca");
}

// ===================== FUNÇÃO PRINCIPAL =====================

void iniciar_interface(int *argc, char ***argv) {
    gtk_init(argc, argv);
    
    // Inicializa a estrutura da aplicação
    WIDGETS *data = g_new0(WIDGETS, 1);
    
    // Conecta no banco de dados
    data->conn = mysql_init(NULL);
    if(!conectar_banco(data->conn)) {
        mostrar_mensagem(NULL, "Erro Fatal", "Não foi possível conectar ao banco de dados", GTK_MESSAGE_ERROR);
        exit(EXIT_FAILURE);
    }
    
    // Cria a janela principal
    data->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(data->window), "RH DIAC - Candidaturas Profissionais");
    gtk_window_set_default_size(GTK_WINDOW(data->window), 600, 400);
    gtk_window_set_position(GTK_WINDOW(data->window), GTK_WIN_POS_CENTER);

    // Abrir em "tela cheia"
    gtk_window_maximize(GTK_WINDOW(data->window));

    
    // Configurar linha do titulo
    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "RH DIAC");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(data->window), header);
    
    // Cria um sistema de navegação
    data->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    // Constroi as telas
    construir_tela_principal(data);
    construir_tela_cadastro(data);
    construir_tela_busca(data);
    
    // Configura a janela
    gtk_container_add(GTK_CONTAINER(data->window), data->stack);
    g_signal_connect(data->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Mostra tudo
    gtk_widget_show_all(data->window);
    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "principal");
    
    // Roda o loop principal
    gtk_main();
    
    // Limpeza
    desconectar_banco(data->conn);
    g_free(data);
}