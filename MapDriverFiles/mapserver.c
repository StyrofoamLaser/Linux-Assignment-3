#include "mapserver.h"
#include "socket_common.h"

#define P_PREFIX "SERVER -"
#define C_PREFIX "SERVER_CHILD -"

int main(int argc, char *argv[])
{
	int listenfd = 0, connfd = 0; /* listenfd is the file descriptor for the socket the server listens to. connfd is the descriptor given when the handshake is done. Represents "file" they are working with. */
	struct sockaddr_in serv_addr;

	FILE* fp = fopen("map_socket.log", "r");
	if (fp != NULL)
	{
		fclose(fp);
		remove("map_socket.log");
	}

	char sendBuff[1025];
	time_t ticks; 

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuff, '0', sizeof(sendBuff)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

	listen(listenfd, 10); 

	while(1)
	{
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 


		/*ticks = time(NULL);
		snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
		write(connfd, sendBuff, strlen(sendBuff));*/

		char* msgValidity = "";

		int pipeFD[2];

		pipe(pipeFD);
		
		int pid = fork();

		char buff[1025];
		int width, height;

		if (pid == 0)
		{
 			/* child stuff */
			int n;

			close(pipeFD[0]); /* our child is only writing to the pipe, close read */

			logz(C_PREFIX, "About to check the socket for a message, and evaluate it's validity.\n");
			if(n = read(connfd, buff, sizeof(buff) - 1) < 0)
			{
				/* print error */
				fprintf(stderr, "ERROR: Error reading from socket.\n");
				logz(C_PREFIX, "Error reading from socket.\n");
				exit(1);	
			}

			msgValidity = interpretMsg(buff, &width, &height);

			//logz(C_PREFIX, msgValidity);

			write(pipeFD[1], msgValidity, sizeof(msgValidity));
			close(pipeFD[1]);

			exit(0);
		}

		wait(0);

		close(pipeFD[1]); /* parent is not writing, close the write */
	
		char str[15];

		read(pipeFD[0], str, sizeof(str));

		close(pipeFD[0]);
		
		//logz(P_PREFIX, str);

		logz(P_PREFIX, "Sending msg to socket based on msg validity\n");

		sendMsg(str, &width, &height, sendBuff, connfd);

		logz(P_PREFIX, "Msg written to socket. Closing connection to client.\n");
		
		close(connfd);
		sleep(1);
	}
}

void sendMsg(char* msgValidity, int *width, int *height, char* sendBuff, int connfd)
{
	if (strcmp(msgValidity, "0") == 0) /* Send a default map message */
	{
		char* deviceMap;
		int fd;

		if((fd = open("/dev/asciimap", O_RDWR)) >= 0)
		{
			int n;
			n = read(fd, deviceMap, 2551);
			
			if (n < 0)
			{
				/* print an error */
				fprintf(stderr, "ERROR: Error reading from /dev/asciimap\n");
				logz(P_PREFIX, "Error reading from /dev/asciimap\n");
			}
			else
			{
				snprintf(sendBuff, sizeof(sendBuff), "%c %i %i %s", PROT_MSG, 50, 50, deviceMap);
				write(connfd, sendBuff, strlen(sendBuff));
				logz(P_PREFIX, "Wrote default /dev/asciimap map to socket.\n");
			}
		}
		else
		{
			logz(P_PREFIX, "Failed to access /dev/asciimap.\n");
			
			char* errMsg = "ERROR: /dev/asciimap could not be accessed.\n";
			snprintf(sendBuff, strlen(sendBuff), "%c %i %s", PROT_ERR, strlen(errMsg), errMsg);

			logz(P_PREFIX, sendBuff);

			write(connfd, sendBuff, strlen(sendBuff));
		}

		close(fd);
	}
	else if (strcmp(msgValidity, "1") == 0) /* Send a custom map message */
	{
		char* generatedMap;

		char* filename = "map_";

		int pid = fork();

		if (pid == 0)
		{
			char pidString[15], widthString[15], heightString[15];
			iToString(getpid(), pidString);	
			iToString(*width, widthString);
			iToString(*height, heightString);

			logz(C_PREFIX, "Generating custom map, about to exec genmap.\n");

			//strcat(filename, pidString);

			execl("genmap.sh", widthString, heightString, filename);
			exit(-1);
		}
		else
		{
			logz(P_PREFIX, "Waiting for genmap.sh child to complete...\n");
			int status;
			waitpid(pid, status, 0);

			logz(P_PREFIX, "Genmap.sh child complete, about to read from given data.\n");
			char pidString[15], widthString[15], heightString[15];
			iToString(getpid(), pidString);
			iToString(*width, widthString);
			iToString(*height, heightString);
			
			//strcat(filename, pidString);
			int fd;
			
			if((fd = open(filename, O_RDWR)) >= 0)
			{
				int n;
				n = read(fd, generatedMap, (*width * *height));

				if (n < 0)
				{
					/* print an error */
					fprintf(stderr, "ERROR: Error reading from generated map file\n");
					logz(P_PREFIX, "Error reading generated map file\n");

					char* errMsg = "ERROR: Error reading from generated map file.0";
					snprintf(sendBuff, strlen(sendBuff), "%c %i %s", PROT_ERR, strlen(errMsg), errMsg);
					write(connfd, sendBuff, strlen(sendBuff));
				}
				else
				{
					snprintf(sendBuff, strlen(sendBuff), "%s %i %i %s", PROT_MSG, widthString, heightString, generatedMap);
					write(connfd, sendBuff, strlen(sendBuff));
					logz(P_PREFIX, "Sending msg to socket with generated map\n");
				}
			}
			else
			{
				logz(P_PREFIX, "Error opening genmap.sh file!\n");
				fprintf(stderr, "ERROR: Error opening file for input!\n");

				char* errMsg = "ERROR: Error opening generated map file.\n";
				snprintf(sendBuff, strlen(sendBuff), "%c %i %s", PROT_ERR, strlen(errMsg), errMsg);
				write(connfd, sendBuff, strlen(sendBuff));
			}

			close(fd);
			remove(filename);
		}
	}
	else /* Send an Error Message */
	{
		char* errMsg = "ERROR: Unrecognized msg protocol.0";
		snprintf(sendBuff, strlen(sendBuff), "%c %i %s", PROT_ERR, strlen(errMsg), errMsg);
		write(connfd, sendBuff, strlen(sendBuff));
		logz(P_PREFIX, "Sending an error msg to socket. Unregistered protocol.\n");
	}
}

char* interpretMsg(char buff[], int *width, int *height)
{
	if (buff[0] == 'M')
	{
		if (buff[2] == '0')
		{
			/* We want a default map to be sent*/
			logz(C_PREFIX, "Msg interpreted as default driver map request. Validity 0.\n");
			return "0";
		}
		else
		{
			/* We need a custom size map, parse the width/height */
			char widthBytes[4] = {buff[2], buff[3], buff[4], buff[5]};
			char heightBytes[4] = {buff[7], buff[8], buff[9], buff[10]};
			*width = atoi(widthBytes);
			*height = atoi(heightBytes);
			
			logz(C_PREFIX, "Msg interpreted as custom size from genmap. Validity 1.\n");
			return "1";
		}
	}
	logz(C_PREFIX, "Msg incorrect. Validity -1.\n");
	return "2";
}

void iToString(int i, char* str)
{
	sprintf(str, "%d", i);
}
