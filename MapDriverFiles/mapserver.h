#ifndef _MAPSERVER_H
#define _MAPSERVER_H
#include <time.h>
#include "socket_common.h"

void sendMsg(int msgValidity, mapmsg_t srcMsg, char* sendBuff, int connfd);
int interpretMsg(mapmsg_t msg);
void iToString(int i, char* str);

#endif
