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

#define DEF_PORT 5000
#define DEF_IP "127.0.0.1"

#define PROT_MSG 'M'
#define PROT_ERR 'E'

/* Globally accessible File Descriptor for the Log File. Use fprintf with this
 * to output log messages. */
extern FILE* LOGFD; 

/* Functions used to open and close the Log File */
void openLogFile();
void closeLogFile();

#endif
