/*
  config_mysql.c
_________________
Théo OMNES
Yanis TAGRI
_________________
08 nov 2020
*/

#include <mysql/mysql.h>
#include "config_mysql.h"

/*__________________________________*/
//connect_to_DB

MYSQL* connect_db(){
  MYSQL *conn;
  char *server = "localhost";
  char *user = "root";
  char *password = "";
  char *database = "test_libc";

  conn = mysql_init(NULL);

  /* Connect to database */
  if (!mysql_real_connect(conn, server,
      user, password, database, 8889, NULL, 0)) {
     fprintf(stderr, "%s\n", mysql_error(conn));
     exit(1);
  }
  return conn;
}

/*__________________________________*/
//close_db
void close_db(MYSQL *conn){
  mysql_close(conn);
}

/*__________________________________*/
//show_tables
MYSQL_RES* show_tables(MYSQL *conn){
  if (mysql_query(conn, "show tables")) {
     fprintf(stderr, "%s\n", mysql_error(conn));
     exit(1);
  }

  return mysql_use_result(conn);
}
/*__________________________________*/
//print_res
void print_res(MYSQL_RES *res, MYSQL_ROW row){
  while ((row = mysql_fetch_row(res)) != NULL)
     printf("%s \n", row[0]);
}
/*__________________________________*/




















/**/
