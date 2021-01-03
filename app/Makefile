kamaji:
	gcc -rdynamic `pkg-config --cflags gtk+-3.0` -o exe/kamaji src/main.c `pkg-config --libs gtk+-3.0` -l mysqlclient
	exe/kamaji

mysql:
	sudo chown -R _mysql:mysql /usr/local/var/mysql
	sudo mysql.server start
