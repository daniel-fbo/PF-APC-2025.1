#include <gtk/gtk.h>  
#include <mysql.h>    
#include "interface.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "estilo.css", NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(cssProvider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    MYSQL *conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "Falha ao inicializar MySQL\n");
        return 1;
    }
    if (!conectar_banco(conn)) {
        fprintf(stderr, "Falha ao conectar ao banco\n");
        return 1;
    }

    iniciar_interface(&argc, &argv);
    mysql_close(conn);
    return 0;
}
