#ifndef _SOCKETCOMMON_H
#define _SOCKETCOMMON_H

#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <syslog.h>

#define DEF_PORT 23032
#define DEF_IP "127.0.0.1"

#define PROT_MSG 'M'
#define PROT_ERR 'E'
#define PROT_KIL 'K'

typedef struct _mapmsg_t
{
	char msgType;
	int param;
	int param2;
	char* map;
} mapmsg_t;

typedef struct _killmsg_t
{
	char msgType;
	int xPos;
	int yPos;
	char initial;
} killmsg_t;

typedef struct _gameovermsg_t
{
	char firstChar;
	char secChar;
} gameovermsg_t;

typedef struct _errmsg_t
{
	char msgType;
	int errLen;
	char* errMsg;
} errmsg_t;

int sizeofM(mapmsg_t msg);
int sizeofK(killmsg_t msg);
int sizeofG(gameovermsg_t);
int sizeofE(errmsg_t msg);

#endif
