#ifndef _MAPSERVER_H
#define _MAPSERVER_H

#include <time.h>

void sendMsg(int msgValidity, int *width, int *height, char* sendBuff, int connfd);
int interpretMsg(char buff[], int *width, int *height);
void iToString(int i, char* str);

#endif
