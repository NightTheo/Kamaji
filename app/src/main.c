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


int main(int argc, char **argv){

  MYSQL *conn = connect_db();
  MYSQL_RES *res;
  res = query(conn, "show tables");

  print_result(res);

  mysql_close(conn);
  return 0;
}
/*END MAIN___________________________________________*/





//
