/*
conf.h
-------------------------
headers of the src/conf.c functions
*/

char *getConf(char *group, char* name);
void removeLF(char *str);
uint8_t fileExists(char *path);
void trim(char *str);
uint8_t isWhiteSpace(char c);
uint8_t isPropertyGroup(char *line, char *group);
