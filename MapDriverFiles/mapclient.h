#ifndef _MAPCLIENT_H
#define _MAPCLIENT_H

#include <netdb.h>

#define LOG_PRFX "Client -"

void printUsage(char* argv);
int sendRequest(int sockfd, char* width, char* height);
int readResponse(int sockfd);

#endif
