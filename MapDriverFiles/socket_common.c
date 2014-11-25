#include "socket_common.h"

FILE* LOGFD = 0;

void openLogFile()
{
	LOGFD = fopen("map_socket.log", "a");

	if (LOGFD < 0)
	{
		fprintf(stderr, "\nError: Could not open Log File.\n");
		exit(1);
	}
}

void closeLogFile()
{
	if (fclose(LOGFD) < 0)
	{
		fprintf(stderr, "\nError: Could not close Log File.\n");
		exit(1);	
	}
}

void logz(char* prefix, char* msg)
{
	openLogFile();
	fprintf(LOGFD, "%s %s", prefix, msg);
	closeLogFile();
}


int getIntFromBuffer(char* buffer, int startIndex, int size)
{
	char charBuff[size];
	int i;
	
	for (i = 0; i < size; i++)
	{
		charBuff[i] = buffer[startIndex + i];
	}

	return atoi(charBuff);
}

int getIntFromRead(int sockfd, char* buffer, char* errPrfx, char* errMsg)
{
	/* Used to cleverly move the buffer pointer to keep adding the integer values to the end of the buffer until a space is hit */
	int i = 0,
	    n = 0;

	/* Read until we hit a space, thats the width. */
	while ((n = read(sockfd, buffer + i, sizeof(char))) > 0)
	{
		if (buffer[i] == ' ')
			break;
		else
			i++;

	}

	if (n < 0)
	{
		fprintf(stderr, "\nError: %s\n", errMsg);
		char* msg = "[Error]: ";
		strcat(msg, errMsg);

		logz(errPrfx, msg);
		return -1;
	}

	return getIntFromBuffer(buffer, 0, i);
}
