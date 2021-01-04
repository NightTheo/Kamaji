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
Get the value of a conf property
-------------------------
 char *group : group of properties, in the conf file is like [group]
 char* property : name of the desired property, in the conf file is like name : value
 -------------------------
 Return value
  char *conf : the property as a string
*/
char *getConf(char *group, char* property){
  char line[256];
  char name[256];
  FILE *fp = fopen(CONF_FILE, "rt");
  char *value = malloc( sizeof(char) * 256 );
  if(value == NULL) exit(1);
  if(fp == NULL) {
    printf("\n\n/!\\Conf file \"%s\" not found ! ~T_T~\n\n", CONF_FILE);
    exit(1);
  }

  *value = '\0';
  while ( fgets(line, 512, fp), !feof(fp)) {
        trim(line);
        if( *line == '#' ) continue; // # is a comment line
        if( isPropertyGroup(line, group) ){
          while (fgets(line, 512, fp), line[0] != '[' && !feof(fp)) { // while not next group or end file
            trim(line);
            if( sscanf(line, "%s : %s", name, value) == 2 ||  line[ strlen(line)-1 ] == ':') // Ends by ':' means empty property
              if( strcmp(name, property) == 0) return value;
            *value = '\0';
          }
          break;
        }
    }

  printf("\n\n/!\\ Conf [%s]%s not found ! ~T_T~\n\n", group, property);
  exit(0);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : getConfInt
-------------------------
Use getConf but return an int
-------------------------
 char *group : group of properties, in the conf file is like [group]
 char* property : name of the desired property, in the conf file is like name : value
 -------------------------
 Return value
  int intConf : the property as an int
*/
int getConfInt(char *group, char* property){
  int intConf;
  char *stringConf;
  char *error;

  stringConf = getConf(group, property);
  intConf = (int)strtol(stringConf, &error, 10 ); // atoi returns 0 when an error append, str to long allows to distinguish this case

  if( error == stringConf ){
    printf("\n\n/!\\ Conf [%s]%s is not an int ! ~T_T~\n\n", group, property);
    free(stringConf);
    exit(0);
  }

  free(stringConf);
  return intConf;
}

/*
-----------------------------------------------------------------------------------------------------------
Function : removeLF
-------------------------
Remove the line feed \n at the end of a string if there is one
-------------------------
 char *str : string to change
*/
void removeLF(char *str){
  if( str != NULL ){
    if( str[ strlen(str) - 1 ] == 10)
       str[ strlen(str) - 1 ] = 0;
  }else{
    printf("Error: sting null");
    exit(0);
  }

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


/*
-----------------------------------------------------------------------------------------------------------
Function : trim
-------------------------
Remove the invisible characters before and after a string
-------------------------
 char *str : string to change
*/
void trim(char *str){
  uint8_t start ;
  uint8_t end ;
  char *tmp;

  if(str == NULL) exit(1);
  tmp = malloc(sizeof(char) * (strlen(str) + 1)) ;

  strcpy(tmp, str) ;
  start = 0 ;
  while ( isWhiteSpace(tmp[start]) )
      start++ ;

  end = strlen(tmp) - 1 ;
  while ( isWhiteSpace(tmp[end]) )
      end-- ;

  strncpy(str, tmp + start, end - start + 1) ;
  str[end - start + 1] = '\0' ;
  free(tmp) ;
}



/*
-----------------------------------------------------------------------------------------------------------
Function : isWhiteSpace
-------------------------
Check if a character is a vertical or horizontal white space :
' ', '\n', '\t', '\f', '\v', '\r'
-------------------------
 char c : char to chech
-------------------------
Return value
  uint8_t : boolean
*/
uint8_t isWhiteSpace(char c){
  char whiteSpace[6] = {' ', '\n', '\t', '\f', '\v', '\r'};

  for(int i = 0; i < 6; i++){
    if( c == whiteSpace[i])
      return 1;
  }

  return 0;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : isPropertyGroup
-------------------------
return 1 if the string is the desired group of properties
-------------------------
char *str : string to compare
char *group : group searched
-------------------------
Return value
  uint8_t : boolean
*/
uint8_t isPropertyGroup(char *str, char *group){
  if( str == NULL || group == NULL) exit(1);
  char tmp[64];

  sprintf(tmp, "[%s]", group);
  if( strcmp(str, tmp) == 0 ) return 1;
  else return 0;
}




















//
