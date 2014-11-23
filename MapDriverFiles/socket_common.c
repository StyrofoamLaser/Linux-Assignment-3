#include "socket_common.h"

int LOGFD = 0;

void openLogFile()
{
	LOGFD = open("map_socket.log", O_RDWR | O_APPEND);

	if (LOGFD < 0)
	{
		fprintf(stderr, "\nError: Could not open Log File.\n");
		exit(1);
	}
}

void closeLogFile()
{
	if (close(LOGFD) < 0)
	{
		fprintf(stderr, "\nError: Could not close Log File.\n");
		exit(1);	
	}
}
