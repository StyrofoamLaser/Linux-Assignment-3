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
