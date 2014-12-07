#ifndef _MAPSERVERG_H
#define _MAPSERVERG_H
#include <time.h>
#include "socket_common.h"

#define LOG_PRFX "SERVER -"
#define LOG_PRFX_C "SERVER CHILD -"

void sendMsg(int msgValidity, char type, void* msg, char* sendBuff, int connfd);
void* readMsg(char type, int connfd);
void writeMsg(char type, void* msg, int connfd);
int interpretMsg(char type, void*  msg);
void iToString(int i, char* str);

#endif
