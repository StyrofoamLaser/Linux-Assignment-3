#ifndef _MAPCLIENTG_H
#define _MAPCLIENTG_H

#include <netdb.h>

#define LOG_PRFX "CLIENT -"

int sendGoMessage(int sockfd);
int sendChildMessage(int sockfd, char initial, int x, int y);
int getXfromIndex(int index, int width);
int getYfromIndex(int index, int width);
void killChildren();
void printMap(char map[]);
int isInFilter(char item, char filter[], int filterSize);
void emptyNonFilterchar(char serverMap[], char filterSet[], int filterSize);
void parseMap(char serverMap[], char* argv[], int mapWidth, int sockfd);

void printUsage(char* argv);
int sendRequest(int sockfd, int width, int height);
int readResponse(int sockfd, char* argv[0]);

#endif
