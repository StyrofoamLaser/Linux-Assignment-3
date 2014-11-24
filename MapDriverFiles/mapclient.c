#include "mapclient.h"
#include "socket_common.h"

int main(int argc, char* argv[])
{
	int sockfd = 0;
	struct sockaddr_in serv_addr;
       	char* ip;
	char* width = "-1";
	char* height = "-1";

	/*openLogFile();*/

	/* If the the user provided too many or too few arguments, give them the usage */
	if(argc > 4 || argc == 2)
	{
		printUsage(argv[0]);
		return 1;
	} 
	else /* Otherwise, determine their request */
	{
		/* If they don't provide an IP, use the default one */
		if (argc == 3 || argc == 1)
			ip = DEF_IP;
		/* Otherwise, use the IP they provide */
		else
			ip = argv[1];

		/* Grab the map bounds */	
		if (argc == 3)
		{
			width = argv[1];
			height = argv[2];
		}
		else if (argc == 4)
		{
			width = argv[2];
			height = argv[3];
		}
	}	

	logz(LOG_PRFX, "Attempting to create socket.\n");

	/* Create the Socket */
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "\nError: Could not create socket.\n");
		logz(LOG_PRFX, "[Error]: Socket creation has failed!\n");
		return 1;
	}

	logz(LOG_PRFX, "Socket successfully created.\n");

	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(DEF_PORT); 

	logz(LOG_PRFX, "Attempting to convert IP Address to Binary.\n");

	/* Convert the IPv4/IPv6 Address from text to binary */
	if(inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
	{
		fprintf(stderr, "\nError: inet_pton Failed.\n");
		logz(LOG_PRFX, "[Error]: IP Address Conversion has failed!\n");
		return 1;
	} 

	logz(LOG_PRFX, "IP Address successfully created.\n");
	logz(LOG_PRFX, "Attempting to Connect to the Server's Socket.\n");

	/* Connect to the server side socket */
	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "\nError: Connection Failed.\n");
		logz(LOG_PRFX, "[Error]: Connection to Server Socket failed!\n");
		return 1;
	} 

	logz(LOG_PRFX, "Server Socket successfully connected to.\n");
	logz(LOG_PRFX, "Attempting to send request to Server.\n");

	/* Create the message based off user input */
	if (sendRequest(sockfd, width, height) < 0)
	{
		fprintf(stderr, "\nError: Request to Server Failed.\n");
		logz(LOG_PRFX, "[Error]: Sending the Request to Server failed!\n");
		return 1;
	}

	logz(LOG_PRFX, "Request successfully sent to Server.\n");
	logz(LOG_PRFX, "Attempting to read response from Server.\n");

	/* Read the response from the server */
	if (readResponse(sockfd) < 0)
	{
		fprintf(stderr, "\nError: An error occured while Reading the server's response.\n");
		logz(LOG_PRFX, "[Error]: Reading the Server's Response has failed!\n");
		return 1;
	}

	logz(LOG_PRFX, "Server Response successfully read.\n");

	/*closeLogFile();*/

	return 0;
}	

void printUsage(char* argv)
{
	printf("\n Usage: %s \n", argv[0]);
	printf("\n Usage: %s <width of map> <height of map>\n", argv[0]);
        printf("\n Usage: %s <ip of server> <width of map> <height of map> \n",argv);
}

int sendRequest(int sockfd, char* width, char* height)
{
	/* Create the message based on the user's input. If the user
	 * specified a width and height, we will send a request for
	 * a map of that size. Otherwise we will send a generic request */
	if (width == "-1" && height == "-1")
	{
		char msgBuff[sizeof(char) * 3 + sizeof(int)];
		/*char msgBuff[1024];*/
		memset(msgBuff, '0', sizeof(msgBuff));
		snprintf(msgBuff, sizeof(msgBuff), "%c %i", PROT_MSG, 0);

		/*char* msgBuff = "M 0    ";*/

		/* Send a message to the server */
		if (write(sockfd, msgBuff, strlen(msgBuff)) < 0)
		{
			fprintf(stderr, "\nError: Writing to server socket failed.\n");
			logz(LOG_PRFX, "[Error]: Writing to Server Socket has failed.\n");
			return -1;
		}
	}
	else
	{
		char msgBuff[sizeof(char) * 4 + sizeof(int) * 2];
		memset(msgBuff, '0', sizeof(msgBuff));
		snprintf(msgBuff, sizeof(msgBuff), "%c %i %i", PROT_MSG, atoi(width), atoi(height));
		
		/* Send a message to the server */
		if (write(sockfd, msgBuff, strlen(msgBuff)) < 0)
		{
			fprintf(stderr, "\nError: Writing to server socket failed.\n");
			logz(LOG_PRFX, "[Error]: Writing to Server Socket has failed.\n");
			return -1;
		}
	}

	return 0;
}

int readResponse(int sockfd)
{
	char recvBuff[1024];
	int n = 0;

	memset(recvBuff, '0',sizeof(recvBuff));

	logz(LOG_PRFX, "Attempting to Read Response Type.\n");

	/* Read the first character, this determines what kind of message it is */
	if (read(sockfd, recvBuff, sizeof(char)) < 0)
	{
		fprintf(stderr, "\nError: Reading server response type failed.\n");
		logz(LOG_PRFX, "[Error]: Reading Server Response Type has failed.\n");
		return -1;
	}

	/* If it is a message, grab its dimensions next */
	if (recvBuff[0] == PROT_MSG)
	{
		logz(LOG_PRFX, "Server Response is of type: Message.\n");
		logz(LOG_PRFX, "Attempting to Read Map Size.\n");

		/* Read the next 11 bytes (3 char for spaces and 2 int for map size) */
		if (read(sockfd, recvBuff, sizeof(char) * 3 + sizeof(int) * 2) < 0)
		{
			fprintf(stderr, "\nError: Reading Map Size failed.\n");
			logz(LOG_PRFX, "[Error]: Reading Map Size has failed.\n");
			return -1;
		}

		logz(LOG_PRFX, "Map Size successfully read.\n");
		logz(LOG_PRFX, "Attempting to read the Map.\n");

		int width = getIntFromBuffer(recvBuff, 2),
		    height = getIntFromBuffer(recvBuff, 7);
		n = 0;
		char map[width * height + height + 1];

		if(read(sockfd, map, sizeof(map)) < 0)
		{
			fprintf(stderr, "\nError: Reading Map failed.\n");
			logz(LOG_PRFX, "[Error]: Reading Map has failed.\n");
			return -1;
		}

		logz(LOG_PRFX, "Map successfully read.\n");

		printf(map);
	}
	/* If it is an error, grab the size of the message next */
	else if (recvBuff[0] == PROT_ERR)
	{
		logz(LOG_PRFX, "Server Response is of type: Error.\n");
		logz(LOG_PRFX, "Attempting to read Error size.\n");

		/* Read the next 6 bytes (2 char for spaces and 1 int for message length) */
		if (read(sockfd, recvBuff, sizeof(char) * 2 + sizeof(int)) < 0)
		{
			fprintf(stderr, "\nError: Reading Error Message length failed.\n");
			logz(LOG_PRFX, "[Error]: Reading Error Message length has failed.\n");
			return -1;
		}

		logz(LOG_PRFX, "Error Message length successfully read.\n");
		logz(LOG_PRFX, "Attempting to read Error Message.\n");

		n = 0;
		int bytesRead = 0;
		int msgSize = getIntFromBuffer(recvBuff, 2);
		char* msg = "";

		printf("Msg Size: %i\n", msgSize);
		printf("%s\n", recvBuff);

		while ( (n = read(sockfd, recvBuff, sizeof(recvBuff) - 1) > 0) && 
			 bytesRead < msgSize)
		{
			bytesRead += n;
			strcat(msg, recvBuff);
		}
		if (n < 0)
		{
			fprintf(stderr, "\nError: Read Error Message failed.\n");
			logz(LOG_PRFX, "[Error]: Read Error Message has failed.\n");
			return -1;
		}

		logz(LOG_PRFX, "Error Message successfully read.\n");

		/* Output the message to STDERR */
		fprintf(stderr, msg);
		logz(LOG_PRFX, msg);
	}
	else
	{
		fprintf(stderr, "\nError: Message Type is unrecognized!\n");
		logz(LOG_PRFX, "[Error]: Message Type is unrecognized!\n");
		printf("WTF: %c\n", recvBuff[0]);

		return -1;
	}

	/* Read from the server socket */
	/*while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
	{
		recvBuff[n] = 0;
		if(fputs(recvBuff, stdout) == EOF)
		{
			printf("\nError: Fputs Failed.\n");
		}

		
	}*/

	/* Return error from read */
	/*if(n < 0)
	{	
		return -1;
	}*/

	return 0;
}

int getIntFromBuffer(char* buffer, int startIndex)
{
	char charBuff[4] = { buffer[startIndex], buffer[startIndex + 1], buffer[startIndex + 2], buffer[startIndex + 3] };

	return atoi(charBuff);
}
