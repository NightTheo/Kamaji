/*
config_mysql.h
-------------------------
headers of the src/config_mysql.c file
*/

MYSQL *connect_db();
MYSQL_RES* query(MYSQL *conn, const char * request );
void print_result(MYSQL_RES *result);
