/*
  main.c
_________________
Théo OMNES
Yanis TAGRI
_________________
last change:
30 nov 2020

Compile:
gcc `pkg-config --cflags gtk+-3.0` -o exe/main src/main.c `pkg-config --libs gtk+-3.0` -l mysqlclient

*/

#include <mysql/mysql.h>
#include <gtk/gtk.h>
#include "config_mysql.c"
#include "retreiveDataFromInput.c"
#include "import_glade.c"

//################################################################
int main(int argc, char **argv){

  gtk_init(&argc, &argv);
  open_home_window("window_home");
  gtk_main();
}
//################################################################



//
