#include "mapserverg.h"
#include "socket_common.h"

#define P_PREFIX "SERVER -"
#define C_PREFIX "SERVER_CHILD -"

int curWidth = 50;
int curHeight = 50;

int main(int argc, char *argv[])
{
	int listenfd = 0, connfd = 0; /* listenfd is the file descriptor for the socket the server listens to. connfd is the descriptor given when the handshake is done. Represents "file" they are working with. */
	struct sockaddr_in serv_addr;

	char sendBuff[1025];

	openlog(LOG_PRFX, LOG_PID, LOG_USER);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuff, '0', sizeof(sendBuff)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(DEF_PORT);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

	listen(listenfd, 10); 

	/*Print map to xterm right here?*/

	while(1)
	{
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 


		/*ticks = time(NULL);
		snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
		write(connfd, sendBuff, strlen(sendBuff));*/

		int msgValidity;
		int pipeFD[2];

		pipe(pipeFD);
		
		int pid = fork();

		if (pid == 0)
		{
 			/* child stuff */
			int n;
			
			close(pipeFD[0]); /* our child is only writing to the pipe, close read */
			
			char type;
			void* msg;
			syslog(LOG_INFO, "About to check the socket for a message, and evaluate it's validity.\n");

			if((n = read(connfd, &type, sizeof(char))) < 0)
			{
				/* print error */
				fprintf(stderr, "ERROR: Error reading type from socket.\n");
				syslog(LOG_INFO, "Error reading type from socket.\n");
				exit(1);	
			}
			else syslog(LOG_INFO, "Read client msg type from socket.\n");			

			msg = readMsg(type, connfd);

			msgValidity = interpretMsg(type, msg);

			write(pipeFD[1], &msgValidity, sizeof(int));
			writeMsg(pipeFD[1], type, msg);	
			close(pipeFD[1]);

			exit(0);
		}

		wait(0);

		close(pipeFD[1]); /* parent is not writing, close the write */
	
		int validity;
		char msgType;
		void* theMsg;

		/* Read the validity int */
		read(pipeFD[0], &validity, sizeof(int));

		/* Read the message type */
		read(pipeFD[0], &msgType, sizeof(char));

		/* read the message itself */
		read(pipeFD[0], &theMsg, sizeof(mapmsg_t));

		close(pipeFD[0]);
		
		/*logz(P_PREFIX, str);*/

		syslog(LOG_INFO, "Sending msg to socket based on msg validity\n");

		sendMsg(validity, msgType, theMsg, sendBuff, connfd);

		syslog(LOG_INFO, "Msg written to socket. Closing connection to client.\n");
		
		sleep(1);
		close(connfd);
	}

	closelog();
}

void sendMsg(int msgValidity, char type, void* msg, char* sendBuff, int connfd)
{
	if(type == 'M')
	{	
		if (msgValidity == 0) /* Send a default map message */
		{
			char deviceMap[curWidth*(curHeight+1)+1];
			int fd;

			if((fd = open("/dev/asciimap", O_RDWR)) >= 0)
			{
				int n;
				n = read(fd, deviceMap, 2551);

				if (n < 0)
				{
					/* print an error */
					fprintf(stderr, "ERROR: Error reading from /dev/asciimap\n");
					syslog(LOG_INFO, "Error reading from /dev/asciimap\n");
				}
				else
				{
					char msgType = PROT_MSG;
					int width = curWidth;
					int height = curWidth;
					char mapArray[sizeof(deviceMap)];
					memcpy(mapArray, deviceMap, sizeof(mapArray));

					if (write(connfd, &msgType, sizeof(char)) < 0)
					{
						fprintf(stderr, "\nError: Writing msg type to Server Socket failed.\n");
						syslog(LOG_ERR, "[Error]: Writing msg type to Server Socket has failed.\n");
						exit(-1);
					}
					else syslog(LOG_INFO, "Wrote default map msgType to socket.\n");

					if (write(connfd, &width, sizeof(int)) < 0)
					{
						fprintf(stderr, "\nError: Writing width to Server Socket failed.\n");
						syslog(LOG_ERR, "[Error]: Writing width to Server Socket has failed.\n");
						exit(-1);
					}
					else syslog(LOG_INFO, "Wrote default map width to socket.\n");

					if (write(connfd, &height, sizeof(int)) < 0)
					{
						fprintf(stderr, "\nError: Writing height to Server Socket failed.\n");
						syslog(LOG_ERR, "[Error]: Writing height to Server Socket has failed.\n");
						exit(-1);
					}
					else syslog(LOG_INFO, "Wrote default map height to socket.\n");

					if (write(connfd, &mapArray, sizeof(mapArray)) < 0)
					{
						fprintf(stderr, "\nError: Writing map to Server Socket failed.\n");
						syslog(LOG_ERR, "[Error]: Writing map to Server Socket has failed.\n");
						exit(-1);
					}
					else syslog(LOG_INFO, "Wrote default map to socket.\n");


					syslog(LOG_INFO, "Wrote default /dev/asciimap map information to socket.\n");
				}
			}
			else
			{
				syslog(LOG_INFO, "Failed to access /dev/asciimap.\n");

				char msgType = PROT_ERR;
				char errLen = 44;
				char errMsg[44];
				memcpy(errMsg, "ERROR: /dev/asciimap could not be accessed.\n", sizeof(errMsg));		

				if (write(connfd, &msgType, sizeof(char)) < 0)
				{
					fprintf(stderr, "\nError: Writing error type to Server Socket failed.\n");
					syslog(LOG_ERR, "[Error]: Writing error type to Server Socket has failed.\n");
					exit(-1);
				}
				else syslog(LOG_INFO, "Wrote /dev/asciimap error type to socket.\n");

				if (write(connfd, &errLen, sizeof(int)) < 0)
				{
					fprintf(stderr, "\nError: Writing error length to Server Socket failed.\n");
					syslog(LOG_ERR, "[Error]: Writing error length to Server Socket has failed.\n");
					exit(-1);
				}
				else syslog(LOG_INFO, "Wrote /dev/asciimap error length to socket.\n");

				if (write(connfd, &errMsg, sizeof(errMsg)) < 0)
				{
					fprintf(stderr, "\nError: Writing error msg to Server Socket failed.\n");
					syslog(LOG_ERR, "[Error]: Writing error msg to Server Socket has failed.\n");
					exit(-1);
				}
				else syslog(LOG_INFO, "Wrote /dev/asciimap error msg to socket\n");
			}

			close(fd);
		}
		else if (msgValidity == 1) /* Send a custom map message */
		{
			int mapSize = ((mapmsg_t*)msg)->param * ((mapmsg_t*)msg)->param2 + ((mapmsg_t*)msg)->param2 + 1;
			char generatedMap[mapSize];

			char* filename = "./map_";

			int pid = fork();

			if (pid == 0)
			{
				char pidString[15], widthString[15], heightString[15];
				iToString(getpid(), pidString);
				iToString(((mapmsg_t*)msg)->param, widthString);
				iToString(((mapmsg_t*)msg)->param2, heightString);

				syslog(LOG_INFO, "Generating custom map, about to exec genmap.\n");

				syslog(LOG_INFO, "%s\n", widthString);
				syslog(LOG_INFO, "%s\n", heightString);

				/*strcat(filename, pidString);*/
				int execStatus;
				execStatus = execl("./genmap.sh", "./genmap.sh", widthString, heightString, filename, NULL);
				/*printf("%i", execStatus);*/
				syslog(LOG_INFO, "ERROR: Exec failed!\n");
				exit(-1);
			}
			else
			{
				syslog(LOG_INFO, "Waiting for genmap.sh child to complete...\n");
				int status = 0;
				waitpid(pid, status, 0);

				syslog(LOG_INFO, "Genmap.sh child complete, about to read from given data.\n");
				char pidString[15], widthString[15], heightString[15];
				iToString(getpid(), pidString);
				iToString(((mapmsg_t*)msg)->param, widthString);
				iToString(((mapmsg_t*)msg)->param2, heightString);

				/*strcat(filename, pidString);*/
				int fd;

				if((fd = open(filename, O_RDWR)) >= 0)
				{
					int n;
					n = read(fd, generatedMap, sizeof(generatedMap));

					if (n < 0)
					{
						/* print an error */
						fprintf(stderr, "ERROR: Error reading from generated map file\n");
						syslog(LOG_INFO, "Error reading generated map file\n");

						char msgType = PROT_ERR;
						int errLen = 46;
						char errmsg[errLen];
						memcpy(errmsg, "ERROR: Error reading from generated map file.\n", sizeof(errmsg));	

						if (write(connfd, &msgType, sizeof(char)) < 0)
						{
							fprintf(stderr, "\nError: Writing error type to Server Socket failed.\n");
							syslog(LOG_ERR, "[Error]: Writing error type to Server Socket has failed.\n");
							exit(-1);
						}
						else syslog(LOG_INFO, "Wrote error type to socket.\n");

						if (write(connfd, &errLen, sizeof(int)) < 0)
						{
							fprintf(stderr, "\nError: Writing error length to Server Socket failed.\n");
							syslog(LOG_ERR, "[Error]: Writing error length to Server Socket has failed.\n");
							exit(-1);
						}
						else syslog(LOG_INFO, "Wrote error type to socket.\n");

						if (write(connfd, &errmsg, sizeof(errmsg)) < 0)
						{
							fprintf(stderr, "\nError: Writing error msg to Server Socket failed.\n");
							syslog(LOG_ERR, "[Error]: Writing error msg to Server Socket has failed.\n");
							exit(-1);
						}
						else syslog(LOG_INFO, "Wrote error msg to socket.\n");
					}
					else
					{
						char msgType = PROT_MSG;
						int width = ((mapmsg_t*)msg)->param;
						int height = ((mapmsg_t*)msg)->param2;
						char map[sizeof(generatedMap)];
						memcpy(map, generatedMap, sizeof(map));

						if (write(connfd, &msgType, sizeof(char)) < 0)
						{
							fprintf(stderr, "\nError: Writing map msg type to Server Socket failed.\n");
							syslog(LOG_ERR, "[Error]: Writing map msg type to Server Socket has failed.\n");
							exit(-1);
						}
						else syslog(LOG_INFO, "Wrote map msg type to socket.\n");

						if (write(connfd, &width, sizeof(int)) < 0)
						{
							fprintf(stderr, "\nError: Writing map width to Server Socket failed.\n");
							syslog(LOG_ERR, "[Error]: Writing map width to Server Socket has failed.\n");
							exit(-1);
						}
						else syslog(LOG_INFO, "Wrote map width to socket.\n");


						if (write(connfd, &height, sizeof(int)) < 0)
						{
							fprintf(stderr, "\nError: Writing map height to Server Socket failed.\n");
							syslog(LOG_ERR, "[Error]: Writing map height to Server Socket has failed.\n");
							exit(-1);
						}
						else syslog(LOG_INFO, "Wrote map height to socket.\n");

						if (write(connfd, &map, sizeof(map)) < 0)
						{
							fprintf(stderr, "\nError: Writing map to Server Socket failed.\n");
							syslog(LOG_ERR, "[Error]: Writing map to Server Socket has failed.\n");
							exit(-1);
						}
						else syslog(LOG_INFO, "Wrote map to socket.\n");

						syslog(LOG_INFO, "Sending msg to socket with generated map\n");
					}
				}
				else
				{
					syslog(LOG_INFO, "Error opening genmap.sh file!\n");
					fprintf(stderr, "ERROR: Error opening file for input!\n");

					char msgType = PROT_ERR;
					int errLen = 51;
					char msg[errLen];
					memcpy(msg, "ERROR: Error opening generated map file.\n", sizeof(msg));	

					if (write(connfd, &msgType, sizeof(char)) < 0)
					{
						fprintf(stderr, "\nError: Writing err type to Server Socket failed.\n");
						syslog(LOG_ERR, "[Error]: Writing err type to Server Socket has failed.\n");
						exit(-1);
					}
					else syslog(LOG_INFO, "Wrote error msg type to socket.\n");

					if (write(connfd, &errLen, sizeof(int)) < 0)
					{
						fprintf(stderr, "\nError: Writing err length to Server Socket failed.\n");
						syslog(LOG_ERR, "[Error]: Writing err length to Server Socket has failed.\n");
						exit(-1);
					}
					else syslog(LOG_INFO, "Wrote error length type to socket.\n");

					if (write(connfd, &msg, sizeof(msg)) < 0)
					{
						fprintf(stderr, "\nError: Writing err msg to Server Socket failed.\n");
						syslog(LOG_ERR, "[Error]: Writing err msg to Server Socket has failed.\n");
						exit(-1);
					}
					else syslog(LOG_INFO, "Wrote error msg to socket.\n");
				}

				close(fd);
				remove(filename);
			}
		}
	}
	else if(type == 'K') /* If the same character exists at the indicated point remove it */
	{
		int arrayPos = ((killmsg_t*)msg)->xPos + (((killmsg_t*)msg)->yPos * curWidth + 1);		
		char readChar[1];

		lseek(connfd, 0, arrayPos);
		read(connfd, readChar, 1);
		/*If char is the same as the selected char change to a space */
		if(readChar[0] == ((killmsg_t*)msg)->initial)
		{
			char writeChar[1];			
			writeChar[0] = ' ';
			lseek(connfd, 0, arrayPos);
			write(connfd, writeChar, 1);
		} 
		/*Pointer reader to front*/
		lseek(connfd, 0, 0);

	}
	else if(type == 'G')
	{
		ioctl(connfd, IOCTL_RESET);		
	}
	else /* Send an Error Message */
	{
		char type = PROT_ERR;
		int errLen = 33;
		char msg[errLen];
		memcpy(msg, "ERROR: Unrecognized msg protocol.", sizeof(msg));

		if (write(connfd, &type, sizeof(char)) < 0)
		{
		}
		else syslog(LOG_INFO, "Wrote error type to socket - unregistered protocol.\n");

		if (write(connfd, &errLen, sizeof(int)) < 0)
		{
		}
		else syslog(LOG_INFO, "Wrote error length to socket - unregistered protocol.\n");

		if (write(connfd, &msg, sizeof(msg)) < 0)
		{
		}
		else syslog(LOG_INFO, "Wrote error msg to socket - unregistered protocol.\n");		

		syslog(LOG_INFO, "Sending an error msg to socket. Unregistered protocol.\n");
	}
}

void* readMsg(char type, int connfd)
{
	void* msgInfo;
	int n;	

	if(type == 'M')
	{
		mapmsg_t* msg;
		int width;
		int height;
		if((n = read(connfd, &width, sizeof(int))) < 0)
		{
			/* print error */
			fprintf(stderr, "ERROR: Error reading width from socket.\n");
			syslog(LOG_INFO, "Error reading width from socket.\n");
			exit(1);	
		}
		else syslog(LOG_INFO, "Read client msg width from socket.\n");

		if (width != 0)
		{
			if((n = read(connfd, &height, sizeof(char))) < 0)
			{
				/* print error */
				fprintf(stderr, "ERROR: Error reading height from socket.\n");
				syslog(LOG_INFO, "Error reading height from socket.\n");
				exit(1);	
			}
			else syslog(LOG_INFO, "Read client msg height from socket.\n");
			msg->param2 = height;
		}
	
		msg->msgType = type;
		msg->param = width;
		msgInfo = msg;
	}

	return msgInfo;
}

void writeMsg(int pipeFD[], char type, void* msg)
{
	switch(type)
	{
		case 'M':
		write(pipeFD[1], &type, sizeof(char));
		write(pipeFD[1], msg, sizeof(mapmsg_t));	
		break;
		case 'K':
		write(pipeFD[1], &type, sizeof(char));
		write(pipeFD[1], msg, sizeof(killmsg_t));
		break;
		case 'G':
		write(pipeFD[1], &type, sizeof(char));
		write(pipeFD[1], msg, sizeof(gameovermsg_t));
		break;
	}
}

int interpretMsg(char type, void* msg)
{
	if (type == 'M')
	{
		
		if (((mapmsg_t*)msg)->param == 0)
		{
			/* We want a default map to be sent*/
			syslog(LOG_INFO, "Msg interpreted as default driver map request. Validity 0.\n");
			return 0;
		}
		else
		{						
			syslog(LOG_INFO, "Msg interpreted as custom size from genmap. Validity 1.\n");
			return 1;
		}
	}
	else if(type == 'K')
	{		
		if(((killmsg_t*)msg)->xPos < curWidth && ((killmsg_t*)msg)->yPos < (curHeight-1) )
		{
			
			syslog(LOG_INFO, "Msg x, y is within boundries of map. Validity 0.");
			return 0;
		}
	}
	else if(type == 'G')
	{
		if(((gameovermsg_t*)msg)->secChar == 'O')
		{
			syslog(LOG_INFO, "Game over message. Validity 0.");
			return 0;
		}
	}
	
	syslog(LOG_INFO, "Msg incorrect. Validity -1.\n");
	return -1;
}

void iToString(int i, char* str)
{
	sprintf(str, "%d", i);
}
