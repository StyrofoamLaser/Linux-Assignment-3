#include "mapserver.h"
#include "socket_common.h"

#define P_PREFIX "SERVER -"
#define C_PREFIX "SERVER_CHILD -"

int main(int argc, char *argv[])
{
	int listenfd = 0, connfd = 0; /* listenfd is the file descriptor for the socket the server listens to. connfd is the descriptor given when the handshake is done. Represents "file" they are working with. */
	struct sockaddr_in serv_addr;

	char sendBuff[1025];
	time_t ticks; 

	openlog(LOG_PRFX, LOG_PID, LOG_USER);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuff, '0', sizeof(sendBuff)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(DEF_PORT);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

	listen(listenfd, 10); 

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

		mapmsg_t msg;
		int width;
		int height;

		if (pid == 0)
		{
 			/* child stuff */
			int n;

			close(pipeFD[0]); /* our child is only writing to the pipe, close read */

			syslog(LOG_INFO, "About to check the socket for a message, and evaluate it's validity.\n");
			if(n = read(connfd, &msg, sizeof(mapmsg_t)) < 0)
			{
				/* print error */
				fprintf(stderr, "ERROR: Error reading from socket.\n");
				syslog(LOG_INFO, "Error reading from socket.\n");
				exit(1);	
			}

			msgValidity = interpretMsg(msg);

			/*logz(C_PREFIX, msgValidity);*/

			write(pipeFD[1], &msgValidity, sizeof(int));
			write(pipeFD[1], &msg, sizeof(mapmsg_t));
			close(pipeFD[1]);

			exit(0);
		}

		wait(0);

		close(pipeFD[1]); /* parent is not writing, close the write */
	
		int validity;
		mapmsg_t theMsg;

		/* Read the validity int */
		read(pipeFD[0], &validity, sizeof(int));

		/* read the message itself */
		read(pipeFD[0], &theMsg, sizeof(mapmsg_t));

		close(pipeFD[0]);
		
		/*logz(P_PREFIX, str);*/

		syslog(LOG_INFO, "Sending msg to socket based on msg validity\n");

		sendMsg(validity, theMsg, sendBuff, connfd);

		syslog(LOG_INFO, "Msg written to socket. Closing connection to client.\n");
		
		close(connfd);
		sleep(1);
	}
	closelog();
}

void sendMsg(int msgValidity, mapmsg_t srcMsg, char* sendBuff, int connfd)
{
	if (msgValidity == 0) /* Send a default map message */
	{
		char deviceMap[1024];
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
				mapmsg_t msg;
				msg.msgType = PROT_MSG;
				msg.param = 50;
				msg.param2 = 50;
				msg.map = deviceMap;

				write(connfd, &msg, sizeof(mapmsg_t));
				syslog(LOG_INFO, "Wrote default /dev/asciimap map to socket.\n");
			}
		}
		else
		{
			syslog(LOG_INFO, "Failed to access /dev/asciimap.\n");
			
			char* errMsg = "ERROR: /dev/asciimap could not be accessed.\n";
			
			errmsg_t error;
			error.msgType = PROT_ERR;
			error.errLen = strlen(errMsg);
			error.errMsg = errMsg;		

			write(connfd, &error, sizeof(errmsg_t));
		}

		close(fd);
	}
	else if (msgValidity == 1) /* Send a custom map message */
	{
		char generatedMap[1024];

		char* filename = "./map_";

		int pid = fork();

		if (pid == 0)
		{
			char pidString[15], widthString[15], heightString[15];
			iToString(getpid(), pidString);
			iToString(srcMsg.param, widthString);
			iToString(srcMsg.param2, heightString);

			syslog(LOG_INFO, "Generating custom map, about to exec genmap.\n");
			char tmp1[40];
			char tmp2[40];

			strcpy(tmp1, widthString);
			strcpy(tmp2, heightString);

			strcat(tmp1, "\n");
			strcat(tmp2, "\n");

			syslog(LOG_INFO, tmp1);
			syslog(LOG_INFO, tmp2);

			/*strcat(filename, pidString);*/
			int execStatus;
			execStatus = execl("./genmap.sh", "./genmap.sh", widthString, heightString, filename, NULL);
			//printf("%i", execStatus);
			syslog(LOG_INFO, "ERROR: Exec failed!\n");
			exit(-1);
		}
		else
		{
			syslog(LOG_INFO, "Waiting for genmap.sh child to complete...\n");
			int status;
			waitpid(pid, status, 0);

			syslog(LOG_INFO, "Genmap.sh child complete, about to read from given data.\n");
			char pidString[15], widthString[15], heightString[15];
			iToString(getpid(), pidString);
			iToString(srcMsg.param, widthString);
			iToString(srcMsg.param2, heightString);
			
			/*strcat(filename, pidString);*/
			int fd;
			
			if((fd = open(filename, O_RDWR)) >= 0)
			{
				int n;
				n = read(fd, generatedMap, 1024);

				if (n < 0)
				{
					/* print an error */
					fprintf(stderr, "ERROR: Error reading from generated map file\n");
					syslog(LOG_INFO, "Error reading generated map file\n");

					char* errMsg = "ERROR: Error reading from generated map file.";					
					errmsg_t error;
					error.msgType = PROT_ERR;
					error.errLen = strlen(errMsg);
					error.errMsg = errMsg;		
	
					write(connfd, &error, sizeof(errmsg_t));
				}
				else
				{
					mapmsg_t msg;
					msg.msgType = PROT_MSG;
					msg.param = srcMsg.param;
					msg.param2 = srcMsg.param2;
					msg.map = generatedMap;

					write(connfd, &msg, sizeof(mapmsg_t));
					syslog(LOG_INFO, "Sending msg to socket with generated map\n");
				}
			}
			else
			{
				syslog(LOG_INFO, "Error opening genmap.sh file!\n");
				fprintf(stderr, "ERROR: Error opening file for input!\n");

				char* errMsg = "ERROR: Error opening generated map file.\n";
				
				errmsg_t error;
				error.msgType = PROT_ERR;
				error.errLen = strlen(errMsg);
				error.errMsg = errMsg;		

				write(connfd, &error, sizeof(errmsg_t));
			}

			close(fd);
			//remove(filename);
		}
	}
	else /* Send an Error Message */
	{
		char* errMsg = "ERROR: Unrecognized msg protocol.";
		
		errmsg_t error;
		error.msgType = PROT_ERR;
		error.errLen = strlen(errMsg);
		error.errMsg = errMsg;

		write(connfd, &error, sizeof(errmsg_t));
		syslog(LOG_INFO, "Sending an error msg to socket. Unregistered protocol.\n");
	}
}

int interpretMsg(mapmsg_t msg)
{
	if (msg.msgType == 'M')
	{
		if (msg.param == 0)
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
	syslog(LOG_INFO, "Msg incorrect. Validity -1.\n");
	return -1;
}

void iToString(int i, char* str)
{
	sprintf(str, "%d", i);
}
