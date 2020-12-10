/*
  config_mysql.c
_________________
Théo OMNES
Yanis TAGRI
_________________
last change:
29 nov 2020
*/

#include <mysql/mysql.h>
#include "../inc/config_mysql.h"

/*__________________________________*/
//connect_to_DB

MYSQL* connect_db(){
  MYSQL *conn;
  char *server = "localhost";
  char *user = "root";
  char *password = "";
  char *database = "kamajiv2";

  conn = mysql_init(NULL);

  /* Connect to database */
  if (!mysql_real_connect(conn, server,
      user, password, database, 8889, NULL, 0)) {
     fprintf(stderr, "%s\n", mysql_error(conn));
     exit(1); //error
  }
  return conn;
}

/*__________________________________*/
//querry
MYSQL_RES* query(MYSQL *conn, const char * request ){
  if (mysql_query(conn, request)) {
     fprintf(stderr, "%s\n", mysql_error(conn));
     exit(1);  //error
  }
  return mysql_use_result(conn);
}
/*__________________________________*/
//print_res
void print_result(MYSQL_RES *result){
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result)) != NULL)
     printf("%s %s\n", row[0], row[1]);
  mysql_free_result(result);
}
/*__________________________________*/




















/**/
