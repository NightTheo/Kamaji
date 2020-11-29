/*
  main.c
_________________
Théo OMNES
Yanis TAGRI
_________________
last change:
29 nov 2020

Compile:
gcc `pkg-config --cflags gtk+-3.0` -o exe/main src/main.c `pkg-config --libs gtk+-3.0` -l mysqlclient

*/

#include <mysql/mysql.h>
#include <gtk/gtk.h>
#include "config_mysql.c"

void homekamaji(int argcounter, char **argvalue);

int main(int argc, char **argv){

  MYSQL *conn = connect_db();
  MYSQL_RES *res;
  res = query(conn, "show tables");
  print_result(res);
  mysql_close(conn);

  GtkBuilder      *builder;
  GtkWidget       *window;

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, "ui/glade/home.glade", NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window_home"));
  gtk_builder_connect_signals(builder, NULL);

  g_object_unref(builder);

  gtk_widget_show(window);
  gtk_main();
}
/*END MAIN___________________________________________*/

void on_window_home_destroy()
{
    gtk_main_quit();
}

//
