kamaji:
	gcc `pkg-config --cflags gtk+-3.0` -o app/kamaji app/src/main.c `pkg-config --libs gtk+-3.0` -l mysqlclient
