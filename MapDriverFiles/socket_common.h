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

typedef struct _errmsg_t
{
	char msgType;
	int errLen;
	char* errMsg;
} errmsg_t;

int sizeofM(mapmsg_t msg);
int sizeofE(errmsg_t msg);

#endif
