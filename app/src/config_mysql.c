/*
  config_mysql.c
_________________
Théo OMNES
Yanis TAGRI
_________________

*/
#include "kamaji.h"

/*__________________________________*/
//connect_to_DB

MYSQL* connect_db(){
  MYSQL *conn;
  char *dbname = getConf("database","dbname");
  char *server = getConf("database","server");
  char *user = getConf("database","login");
  char *password = getConf("database","password");

  conn = mysql_init(NULL);

  /* Connect to database */
  if (!mysql_real_connect(conn, server,
      user, password, dbname, 8889, NULL, 0)) {
     fprintf(stderr, "%s\n", mysql_error(conn));
     exit(1); //error
  }

  free(dbname); free(server); free(user); free(password);
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
