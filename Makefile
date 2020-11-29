kamaji:
	gcc `pkg-config --cflags gtk+-3.0` -o app/exe/main app/src/main.c `pkg-config --libs gtk+-3.0` -l mysqlclient
