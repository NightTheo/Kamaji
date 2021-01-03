APPNAME = kamaji

#Compilation flags
CFLAGS =  -rdynamic `pkg-config --cflags gtk+-3.0`

#Libraries
LIBS = `pkg-config --libs gtk+-3.0` -l mysqlclient

#C files
CFILES = src/main.c src/navigation.c src/insertDataGtk.c  src/config_mysql.c

#Executable
EXE = exe/$(APPNAME)

kamaji:
	gcc $(CFLAGS) -o $(EXE) $(CFILES) $(LIBS)

mysql:
	sudo chown -R _mysql:mysql /usr/local/var/mysql
	sudo mysql.server start
