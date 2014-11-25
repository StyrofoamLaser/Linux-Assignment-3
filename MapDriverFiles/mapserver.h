#ifndef _MAPSERVER_H
#define _MAPSERVER_H
#include <time.h>

void sendMsg(char* msgValidity, int *width, int *height, char* sendBuff, int connfd);
char* interpretMsg(char buff[], int *width, int *height);
void iToString(int i, char* str);

#endif
