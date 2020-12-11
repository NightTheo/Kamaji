/*
  config_mysql.h
_________________
Théo OMNES
Yanis TAGRI
_________________

*/

MYSQL *connect_db();
MYSQL_RES* query(MYSQL *conn, const char * request );
void print_result(MYSQL_RES *result);
