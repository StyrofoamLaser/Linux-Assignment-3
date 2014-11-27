#ifndef _MAPCLIENT_H
#define _MAPCLIENT_H

#include <netdb.h>

#define LOG_PRFX "CLIENT -"

void printUsage(char* argv);
int sendRequest(int sockfd, int width, int height);
int readResponse(int sockfd);

#endif
