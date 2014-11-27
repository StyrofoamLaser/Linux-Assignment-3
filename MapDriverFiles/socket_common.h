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

/* Fix port so it runs on 23032 */
#define DEF_PORT 23032
#define DEF_IP "127.0.0.1"

#define PROT_MSG 'M'
#define PROT_ERR 'E'

/* Globally accessible File Descriptor for the Log File. Use fprintf with this
 * to output log messages. */
extern FILE* LOGFD;

/* Functions used to open and close the Log File */
void openLogFile();
void closeLogFile();
void logz(char* prefix, char* msg);
int getIntFromBuffer(char* buffer, int startIndex, int size);
int getIntFromRead(int sockfd, char* buffer, char* errPrfx, char* errMsg);

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

#endif
