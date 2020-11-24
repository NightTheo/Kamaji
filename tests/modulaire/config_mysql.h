/*
  config_mysql.h
_________________
Théo OMNES
Yanis TAGRI
_________________
08 nov 2020
*/

MYSQL* connect_db();
void close_db(MYSQL *conn);
MYSQL_RES* show_tables(MYSQL *conn);
void print_res(MYSQL_RES *res, MYSQL_ROW row);
