#ifndef _MAPSERVERG_H
#define _MAPSERVERG_H
#include <time.h>
#include "socket_common.h"
#include <sys/ioctl.h>

#define LOG_PRFX "SERVER -"
#define LOG_PRFX_C "SERVER CHILD -"

#define MAJOR_NUMBER 248;

#define IOCTL_RESET _IOR(248, 0, int)

void sendMsg(int msgValidity, char type, void* msg, char* sendBuff, int connfd);
void* readMsg(char type, int connfd);
void writeMsg(int pipeFD, char type, void* msg);
int interpretMsg(char type, void*  msg);
void iToString(int i, char* str);
void printMap();

#endif
