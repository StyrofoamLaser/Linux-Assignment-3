#include "mapserver.h"
#include "socket_common.h"

int main(int argc, char *argv[])
{
	int listenfd = 0, connfd = 0; /* listenfd is the file descriptor for the socket the server listens to. connfd is the descriptor given when the handshake is done. Represents "file" they are working with. */
	struct sockaddr_in serv_addr;

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

		int pid = fork();

		char buff[1025];
		int width, height;
		int msgValidity;

		int pipeFD[2];

		pipe(pipeFD);

		if (pid == 0)
		{
 			/* child stuff */
			int n;

			close(pipeFD[0]); /* our child is only writing to the pipe, close read */

			while ((n = read(connfd, buff, sizeof(buff) - 1) > 0))
			{
				msgValidity = interpretMsg(buff, &width, &height);
			}

			if (n < 0)
			{
				/* print error */
				fprintf(stderr, "ERROR: Error reading from socket.");
				exit(1);
			}

			write(pipeFD[1], &msgValidity, sizeof(msgValidity);
			close(pipeFD[1]);

			exit(0);
		}

		wait(0);

		close(pipeFD[1]); /* parent is not writing, close the write */

		read(pipeFD[0], &msgValidity, sizeof(int));

		close(pipeFD[0]);

		sendMsg(msgValidity, &width, &height, sendBuff, connfd);
		
		close(connfd);
		sleep(1);
	}
}

void sendMsg(int msgValidity, int *width, int *height, char* sendBuff, int connfd)
{
	if (msgValidity == 0) /* Send a default map message */
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
				fprintf(stderr, "ERROR: Error reading from /dev/asciimap");
			}
			else
			{
				snprintf(sendBuff, sizeof(sendBuff), "%c %i %i %s", PROT_MSG, 50, 50, deviceMap);
				write(connfd, sendBuff, strlen(sendBuff));
			}
		}

		close(fd);
	}
	else if (msgValidity == 1) /* Send a custom map message */
	{
		char* generatedMap;

		int pid = fork();

		char* filename = "map_";

		if (pid == 0)
		{
			char pidString[15], widthString[15], heightString[15];
			iToString(getpid(), pidString);	
			iToString(*width, widthString);
			iToString(*height, heightString);	
			
			strcat(filename, pidString);
			execl("./genmap.sh", widthString, heightString, filename);
		}
		else
		{
			wait(0);

			char pidString[15], widthString[15], heightString[15];
			iToString(getpid(), pidString);
			iToString(*width, widthString);
			iToString(*height, heightString);
			
			strcat(filename, pidString);
			int fd;
			
			if((fd = open(filename, O_RDWR)) >= 0)
			{
				int n;
				n = read(fd, generatedMap, (*width * *height));

				if (n < 0)
				{
					/* print an error */
					fprintf(stderr, "ERROR: Error reading from generated map file");
				}
				else
				{
					snprintf(sendBuff, sizeof(sendBuff), "%s %i %i %s", PROT_MSG, widthString, heightString, generatedMap);
					write(connfd, sendBuff, strlen(sendBuff));
				}
			}

			close(fd);
			remove(filename);
		}
	}
	else /* Send an Error Message */
	{
		char* errMsg = "ERROR: Unrecognized msg protocol.0";
		snprintf(sendBuff, sizeof(sendBuff), "%c %i %s", PROT_ERR, sizeof(errMsg), errMsg);
		write(connfd, sendBuff, strlen(sendBuff));
	}
}

int interpretMsg(char buff[], int *width, int *height)
{
	if (buff[0] == 'M')
	{
		if (buff[2] == 0)
		{
			/* We want a default map to be sent*/
			return 0;
		}
		else
		{
			/* We need a custom size map, parse the width/height */
			char widthBytes[4] = {buff[2], buff[3], buff[4], buff[5]};
			char heightBytes[4] = {buff[7], buff[8], buff[9], buff[10]};
			*width = atoi(widthBytes);
			*height = atoi(heightBytes);
			return 1;
		}
	}
	else
	{
		return -1;
	}
}

void iToString(int i, char* str)
{
	sprintf(str, "%d", i);
}
