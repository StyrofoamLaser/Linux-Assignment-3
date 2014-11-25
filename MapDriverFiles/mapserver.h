#ifndef _MAPSERVER_H
#define _MAPSERVER_H
#include <time.h>

void sendMsg(char* msgValidity, char* width, char* height, char* sendBuff, int connfd);
char* interpretMsg(char buff[], char* width, char* height);
void iToString(int i, char* str);
int getIntFromBuffer(char* buffer, int startIndex, int size);

#endif
