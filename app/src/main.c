/*
  main.c
_________________
Théo OMNES
Yanis TAGRI
_________________

Compile:
gcc `pkg-config --cflags gtk+-3.0` -o exe/main src/main.c `pkg-config --libs gtk+-3.0` -l mysqlclient

*/

#include <mysql/mysql.h>
#include <gtk/gtk.h>

#include "config_mysql.c"
#include "navigation.c"
#include "insertDataGtk.c"


//################################################################
int main(int argc, char **argv){
  Session *session;

  gtk_init(&argc, &argv);

  session = malloc(sizeof(Session));
  if( session == NULL ) exit(1);

  open_home_window(NULL, session);
  gtk_main();
}
//################################################################



//
