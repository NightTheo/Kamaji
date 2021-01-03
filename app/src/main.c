/*
  main.c
_________________
Théo OMNES
Yanis TAGRI
_________________

Compile:
gcc `pkg-config --cflags gtk+-3.0` -o exe/main src/main.c `pkg-config --libs gtk+-3.0` -l mysqlclient

*/

#include "kamaji.h"


//################################################################
int main(int argc, char **argv){
  Session *session;
  time_t now;

  char *conf = getConf("","");
  free(conf); conf = NULL;

  gtk_init(&argc, &argv);

  session = initSession();
  time( &now );
  session->today = localtime( &now ); // get the current date as tm struct

  open_home_window(NULL, session);

  gtk_main();
}
//################################################################



//
