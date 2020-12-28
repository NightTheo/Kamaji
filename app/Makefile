kamaji:
	gcc -rdynamic `pkg-config --cflags gtk+-3.0` -o exe/kamaji src/main.c `pkg-config --libs gtk+-3.0` -l mysqlclient
	exe/kamaji

clean:
	rm exe/*
