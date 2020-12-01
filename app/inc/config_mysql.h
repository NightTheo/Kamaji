/*
  config_mysql.h
_________________
Théo OMNES
Yanis TAGRI
_________________
last change:
29 nov 2020
*/

MYSQL *connect_db();
MYSQL_RES* query(MYSQL *conn, const char * request );
void print_result(MYSQL_RES *result);
