#include "mapclientg.h"
#include "socket_common.h"
#include <signal.h>

/*
 * Parent keeps a dynamic array of PIDs of all its children
 * to wait for them afterwards.
 */
pid_t* piChildrenPIDs = NULL;

/*Part 5 functions*/

int sendGoMessage(int sockfd)
{
	char msgType = PROT_GO;
	char letter = '0';
	
	syslog(LOG_INFO, "Attempting to write Message Type to Server Socket.\n");

        if (write(sockfd, &msgType, sizeof(char)) < 0)
        {
                fprintf(stderr, "\nError: Writing Message Type to Server Socket failed.\n");
                syslog(LOG_ERR, "[Error]: Writing Message Type to Server Socket has failed.\n");
                return -1;
        }

        syslog(LOG_INFO, "Write was successful.\n");

	syslog(LOG_INFO, "Attempting to write one letter to Server Socket.\n");

        if (write(sockfd, &letter, sizeof(char)) < 0)
        {
                fprintf(stderr, "\nError: Writing letter to Server Socket failed.\n");
                syslog(LOG_ERR, "[Error]: Writing letter to Server Socket has failed.\n");
                return -1;
        }

        syslog(LOG_INFO, "Write was successful.\n");

	return 0;
}

int sendChildMessage(int sockfd, char initial, int x, int y)
{	
	char msgType = PROT_KIL;

	syslog(LOG_INFO, "Attempting to write Message Type to Server Socket.\n");

	if (write(sockfd, &msgType, sizeof(char)) < 0)
	{
		fprintf(stderr, "\nError: Writing Message Type to Server Socket failed.\n");
		syslog(LOG_ERR, "[Error]: Writing Message Type to Server Socket has failed.\n");
		return -1;
	}

	syslog(LOG_INFO, "Write was successful.\n");
	syslog(LOG_INFO, "Attempting to write initial to Server Socket.\n");

	if (write(sockfd, &initial, sizeof(char)) < 0)
	{
		fprintf(stderr, "\nError: Writing initial to Server Socket failed.\n");
		syslog(LOG_ERR, "[Error]: Writing initial to Server Socket has failed.\n");
		return -1;
	}

	syslog(LOG_INFO, "Write was successful.\n");

	syslog(LOG_INFO, "Attempting to write x to Server Socket.\n");

	if (write(sockfd, &x, sizeof(int)) < 0)
	{
		fprintf(stderr, "\nError: Writing x to Server Socket failed.\n");
		syslog(LOG_ERR, "[Error]: Writing x to Server Socket has failed.\n");
		return -1;
	}

	syslog(LOG_INFO, "Write was successful.\n");

	syslog(LOG_INFO, "Attempting to write y to Server Socket.\n");

        if (write(sockfd, &y, sizeof(int)) < 0)
        {
                fprintf(stderr, "\nError: Writing y to Server Socket failed.\n");
                syslog(LOG_ERR, "[Error]: Writing y to Server Socket has failed.\n");
                return -1;
        }

	syslog(LOG_INFO, "Write was successful.\n");

	return 0;
}

int getXfromIndex(int index, int width)
{
        return index % width;
}

int getYfromIndex(int index, int width)
{
        return index / width;
}

void killChildren()
{
	int i;
	for(i = 0; i < sizeof(piChildrenPIDs); i++)
	{
		if(piChildrenPIDs[i] != 0)
                {
                       	kill(piChildrenPIDs[i], SIGKILL);
                }	
	}
	syslog(LOG_INFO, "Succesfully killed all children\n");
}

static void sig_end(int signo)
{
        if(signo == SIGINT)
        {
		syslog(LOG_INFO, "Caught SIGINT\n");
                killChildren();
                exit(0);
        }
        else if(signo == SIGHUP)
        {
		syslog(LOG_INFO, "Caught SIGHUP\n");
                killChildren();
                exit(0);
        }
	else if(signo == SIGKILL)
	{
		syslog(LOG_INFO, "Caught SIGKILL\n");
		killChildren();
                exit(0);
	}
        else {
		syslog(LOG_ERR, "Caught an unknown signal.\n");
                fprintf(stderr, "received signal: %d\n", signo);
                exit(1);
        }
        return;
}

static void sig_usr(int signo) {
        if (signo == SIGUSR1)
	{
		syslog(LOG_INFO, "Caught SIGUSR1\n");
                printf("received SIGUSR1\n");
        }
	else {
		syslog(LOG_ERR, "Caught an unkown signal.\n");
                fprintf(stderr, "received signal: %d\n", signo);
                exit(1);
        }
        return;
}

static void exit_handler()
{
        printf("exit\n");

        /*Send signal*/

	syslog(LOG_INFO, "Sent GO signal. Not actually yet implemented.\n");
}

void printMap(char map[])
{
        printf("\n-----\n");
        printf(map);
        printf("\n-----\n");
}

int isInFilter(char item, char filter[], int filterSize)
{
        if(item == '\n' || item == '\0')
        {
                return 1;
        }

        int i;

        for(i = 0; i < filterSize; i++)
        {
                if(item == filter[i])
                {
                        return 1;
                }
        }

        return 0;
}

void emptyNonFilterchar(char serverMap[], char filterSet[], int filterSize)
{
        int i;

        for(i = 0; i < sizeof(serverMap); i++)
        {
                if(isInFilter(serverMap[i], filterSet, filterSize) == 0)
                {
                        serverMap[i] = ' ';
                }
        }
}

void parseMap(char serverMap[], char* argv[], int mapWidth)
{
	atexit(exit_handler);
        if(signal(SIGINT, sig_end) == SIG_ERR)
        {
                printf("Can't catch signal\n");
		syslog(LOG_ERR, "could not set signal handler.\n");
        }
        if(signal(SIGHUP, sig_end) == SIG_ERR)
        {
                printf("Can't catch signal\n");
		syslog(LOG_ERR, "could not set signal handler.\n");
        }
	if(signal(SIGKILL, sig_end) == SIG_ERR)
	{
		printf("Can't catch signal\n");
		syslog(LOG_ERR, "could not set signal handler.\n");
	}

        /*
         * Parent keeps a dynamic array of PIDs of all its children
         * to wait for them afterwards.
         */
        pid_t* piChildrenPIDs = NULL;

        /*
         * Next child's PID
         */
        pid_t iChildPID;
        int numChilds = 0;

        int i;
        int j;

        char filterSet[] = {'J', 'P', 'V', 'L', 'C', 'R', 'H', 'S', 'B', 'D'};
        int filterSize = 10;

        /*
         * Allocate as much storage as entries there are in the map
         */
        piChildrenPIDs = malloc(sizeof(serverMap));

        emptyNonFilterchar(serverMap, filterSet, filterSize);
        printMap(serverMap);
        for(i = 0; i < filterSize; i++)
        {
                for(j = 0; j < sizeof(serverMap); j++)
                {
                        if(serverMap[j] == filterSet[i])
                        {
                                iChildPID = fork();
				syslog(LOG_INFO, "Forked child with PID %d.\n", iChildPID);

                                if(iChildPID == 0)
                                {
                                        strcpy(argv[0], "teampid ");
                                        strncat(argv[0], &filterSet[i], 1);
                                        strncat(argv[0], " ", 1);
                                        char x[11];
                                        char y[11];
                                        snprintf(x, 11, "%d", getXfromIndex(j, mapWidth));
                                        strncat(argv[0], x, 11);

                                        strncat(argv[0], " ", 1);

                                        snprintf(y, 11, "%d", getYfromIndex(j, mapWidth));
                                        strncat(argv[0], y, 11);

                                        printf(argv[0]);
                                        /*Set signal handler*/
                                        if(signal(SIGUSR1, sig_usr) == SIG_ERR)
                                        {
                                                printf("Can't catch signal\n");
						syslog(LOG_ERR, "could not set signal handler.\n");
                                        }

                                        pause();

                                        /*Send server message*/
					
					syslog(LOG_INFO, "Sent K signal. not yet actually implemented\n");
                                        /*Exit*/
					syslog(LOG_INFO, "Child exiting.\n");
                                        exit(filterSet[i]);
                                }
                                else if(iChildPID > 0)
                                {
                                        /*stuff the parent does*/
                                        /* store PID of the just forked child */
                                        piChildrenPIDs[j] = iChildPID;
                                        numChilds += 1;
                                }
                                else
                                {
                                        /*Fork failed*/
					syslog(LOG_ERR, "Fork failed.\n");
                                        perror("Fork failed");
                                }
                        }
                }
        }

        while(numChilds > 0)
        {
                for(i = 0; i < sizeof(serverMap); i++)
                {
                        if(waitpid(piChildrenPIDs[i], NULL, WNOHANG) != 0)
                        {
                                /*printf("%dded\n", piChildrenPIDs[i]);*/
                                if(serverMap[i] != ' ' && serverMap[i] != '\n' && serverMap[i] != '\0')
                                {
                                        serverMap[i] = ' ';
                                        printMap(serverMap);
                                        numChilds -= 1;
                                }
                        }
                        else
                        {
                                /*printf("%dliv\n", piChildrenPIDs[i]);*/
                        }
                }
        }
}

/*Part 4 functions*/
int main(int argc, char* argv[])
{
	int sockfd = 0;
	struct sockaddr_in serv_addr;
       	char* ip;
	int width = -1;
	int height = -1;

	openlog(LOG_PRFX, LOG_PID, LOG_USER);

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
			width = atoi(argv[1]);
			height = atoi(argv[2]);
		}
		else if (argc == 4)
		{
			width = atoi(argv[2]);
			height = atoi(argv[3]);
		}
	}	

	syslog(LOG_INFO, "Attemping to create socket.\n");

	/* Create the Socket */
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "\nError: Could not create socket.\n");
		syslog(LOG_ERR, "[Error]: Socket Creation has failed.\n");
		return 1;
	}

	syslog(LOG_INFO, "Socket successfully created.\n");

	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(DEF_PORT); 

	syslog(LOG_INFO, "Attempting to convert IP Address to Binary.\n");

	/* Convert the IPv4/IPv6 Address from text to binary */
	if(inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
	{
		fprintf(stderr, "\nError: inet_pton Failed.\n");
		syslog(LOG_ERR, "[Error]: IP Address Conversion has failed!\n");
		return 1;
	} 

	syslog(LOG_INFO, "IP Address successfully created.\n");
	syslog(LOG_INFO, "Attempting to Connect to the Server's Socket.\n");

	/* Connect to the server side socket */
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "\nError: Connection Failed.\n");
		syslog(LOG_ERR, "[Error]: Connection to Server Socket failed!\n");
		return 1;
	} 

	syslog(LOG_INFO, "Server Socket successfully connected to.\n");
	syslog(LOG_INFO, "Attempting to send request to Server.\n");

	/* Create the message based off user input */
	if (sendRequest(sockfd, width, height) < 0)
	{
		fprintf(stderr, "\nError: Request to Server Failed.\n");
		syslog(LOG_ERR, "[Error]: Sending the Request to Server failed!\n");
		return 1;
	}

	syslog(LOG_INFO, "Request successfully sent to Server.\n");
	syslog(LOG_INFO, "Attempting to read response from Server.\n");

	syslog(LOG_INFO, "Sleeping to wait for server response.\n");
	/* Sleep to allow the server time to write the map info. */
	sleep(3);

	/* Read the response from the server */
	if (readResponse(sockfd, argv) < 0)
	{
		fprintf(stderr, "\nError: An error occured while Reading the server's response.\n");
		syslog(LOG_ERR, "[Error]: Reading the Server's Response has failed!\n");
		return 1;
	}

	syslog(LOG_INFO, "Server Response successfully read.\n");

	closelog();

	return 0;
}	

void printUsage(char* argv)
{
	printf("\n Usage: %s \n", argv);
	printf("\n Usage: %s <width of map> <height of map>\n", argv);
        printf("\n Usage: %s <ip of server> <width of map> <height of map> \n", argv);
}

int sendRequest(int sockfd, int width, int height)
{
	char msgType = PROT_MSG;
	int param = 0;
	int param2 = 0;
	
	/* Create the message based on the user's input. If the user
	 * specified a width and height, we will send a request for
	 * a map of that size. Otherwise we will send a generic request. */
	if (width != -1 && height != -1)
	{
		param = width;
		param2 = height;
	}

	syslog(LOG_INFO, "Attempting to write Message Type to Server Socket.\n");

	if (write(sockfd, &msgType, sizeof(char)) < 0)
	{
		fprintf(stderr, "\nError: Writing Message Type to Server Socket failed.\n");
		syslog(LOG_ERR, "[Error]: Writing Message Type to Server Socket has failed.\n");
		return -1;
	}

	syslog(LOG_INFO, "Write was successful.\n");
	syslog(LOG_INFO, "Attempting to write Param 1 to Server Socket.\n");

	if (write(sockfd, &param, sizeof(int)) < 0)
	{
		fprintf(stderr, "\nError: Writing Param 1 to Server Socket failed.\n");
		syslog(LOG_ERR, "[Error]: Writing Param 1 to Server Socket has failed.\n");
		return -1;
	}

	syslog(LOG_INFO, "Write was successful.\n");

	/* If the first param was never set, the second param was never set and 
	 * we don't need to continue. Otherwise, write the second parameter */
	if (param == 0)
		return 0;

	syslog(LOG_INFO, "Attempting to write Param 2 to Server Socket.\n");

	if (write(sockfd, &param2, sizeof(int)) < 0)
	{
		fprintf(stderr, "\nError: Writing Param 2 to Server Socket failed.\n");
		syslog(LOG_ERR, "[Error]: Writing Param 2 to Server Socket has failed.\n");
		return -1;
	}

	syslog(LOG_INFO, "Write was successful.\n");

	return 0;
}

int readResponse(int sockfd, char* argv[])
{
	syslog(LOG_INFO, "Attempting to Read Response Type.\n");

	char msgType = ' ';

	/* Read the first character, this determines what kind of message it is */
	if (read(sockfd, &msgType, sizeof(char)) < 0)
	{
		fprintf(stderr, "\nError: Reading Server Response Type failed.\n");
		syslog(LOG_ERR, "[Error]: Reading Server Response Type has failed.\n");
		return -1;
	}

	/* If it is a message, grab its dimensions next */
	if (msgType == PROT_MSG)
	{
		syslog(LOG_INFO, "Server Response is of type: Message.\n");
		syslog(LOG_INFO, "Attempting to Read Map Size.\n");	

		int width = 0,
		    height = 0;	
		
		/* Read the Width */
		if (read(sockfd, &width, sizeof(int)) < 0)
		{
			fprintf(stderr, "\nError: Read Map Width failed.\n");
			syslog(LOG_ERR, "[Error]: Read Map Width has failed.\n");
			return -1;
		}

		/* Read the Height */
		if (read(sockfd, &height, sizeof(int)) < 0)
		{
			fprintf(stderr, "\nError: Read Map Height failed.\n");
			syslog(LOG_ERR, "[Error]: Read Map Height has failed.\n");
			return -1;
		}

		syslog(LOG_INFO, "Map Size successfully read.\n");
		syslog(LOG_INFO, "Attempting to read the Map.\n");

		int mapSize =  width * height + height + 1;
		char map[mapSize];
		memset(map, '0', sizeof(map));
		
		/* Reads for exactly the map size into our map. */
		if(read(sockfd, &map, sizeof(map)) < 0)
		{
			fprintf(stderr, "\nError: Reading Map failed.\n");
			syslog(LOG_ERR, "[Error]: Reading Map has failed.\n");
			return -1;
		}

		syslog(LOG_INFO, "Map successfully read.\n");

		printf(map);
		
		parseMap(map, argv, width + 1);
	}
	/* If it is an error, grab the size of the message next */
	else if (msgType == PROT_ERR)
	{
		syslog(LOG_INFO, "Server Response is of type: Error.\n");
		syslog(LOG_INFO, "Attempting to read Error size.\n");

		int msgSize = 0;

		/* Read the message length */
		if (read(sockfd, &msgSize, sizeof(int)) < 0)
		{
			fprintf(stderr, "\nError: Read Error Message Length failed.\n");
			syslog(LOG_ERR, "[Error]: Read Error Message Length has failed.\n");
			return -1;
		}
		
		syslog(LOG_INFO, "Error Message length successfully read.\n");
		syslog(LOG_INFO, "Attempting to read Error Message.\n");

		char errMsg[msgSize];
		memset(errMsg, '0', sizeof(errMsg));

		/* Read the message into our char* */
		if (read(sockfd, &errMsg, sizeof(errMsg)) < 0)
		{
			fprintf(stderr, "\nError: Read Error Message failed.\n");
			syslog(LOG_ERR, "[Error]: Read Error Message has failed.\n");
			return -1;
		}

		syslog(LOG_INFO, "Error Message successfully read.\n");

		/* Output the message to STDERR */
		fprintf(stderr, errMsg);
		syslog(LOG_INFO, "%s\n", errMsg);
	}
	else
	{
		fprintf(stderr, "\nError: Message Type '%c' is unrecognized!\n", msgType);
		syslog(LOG_ERR, "[Error]: Message Type '%c' is unrecognized!\n", msgType);
		return -1;
	}

	return 0;
}
