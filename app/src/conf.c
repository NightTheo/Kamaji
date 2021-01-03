/*
conf.c
-------------------------
Get a property from the conf file /conf.txt
*/
#include "kamaji.h"


/*
-----------------------------------------------------------------------------------------------------------
Function : getConf
-------------------------
Open the window with the selection of place and room as you want for reservation
-------------------------
 GtkWidget *widget: widget activated to open the window, here the button_home_calendars button
 Session *session : address of the struct Session
 -------------------------
 Return value
  char *conf;
*/
char *getConf(char *group, char* property){
  FILE *fp = fopen();
}


/*
-----------------------------------------------------------------------------------------------------------
Function : fileExists
-------------------------
says if the given path is a file, at least an readable file
-------------------------
char *path : path of the file
-------------------------
Return values
    uint8_t exists : boolean, 1 if the file exists, else 0
*/
uint8_t fileExists(char *path){
  uint8_t exists;
  FILE *fp = fopen( path, "r" );
  exists = fp != NULL ? 1 : 0;
  fclose(fp);
  return exists;
}
