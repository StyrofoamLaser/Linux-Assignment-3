#ifndef _MAPCLIENT_H
#define _MAPCLIENT_H

#include <netdb.h>

void printUsage(char* argv);
int sendRequest(int sockfd, char* width, char* height);
int readResponse(int sockfd);
int getIntFromBuffer(char* buffer, int startIndex);

#endif
