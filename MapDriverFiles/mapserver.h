#ifndef _MAPSERVER_H
#define _MAPSERVER_H
#include <time.h>
#include "socket_common.h"

#define LOG_PRFX "SERVER -"
#define LOG_PRFX_C "SERVER CHILD -"

void sendMsg(int msgValidity, mapmsg_t srcMsg, char* sendBuff, int connfd);
int interpretMsg(mapmsg_t msg);
void iToString(int i, char* str);

#endif
