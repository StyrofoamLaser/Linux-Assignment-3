#ifndef _MAPCLIENTG_H
#define _MAPCLIENTG_H

#include <netdb.h>

#define LOG_PRFX "CLIENT -"

void printUsage(char* argv);
int sendRequest(int sockfd, int width, int height);
int readResponse(int sockfd, char* argv[0]);

#endif
