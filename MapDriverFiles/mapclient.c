#include "mapclient.h"
#include "socket_common.h"

int main(int argc, char* argv[])
{
	int sockfd = 0, n = 0;
    	char recvBuff[1024];
	struct sockaddr_in serv_addr;
       	char* ip;	

	/* If the the user provided too many arguments, give them the usage */
    	if(argc > 2)
    	{
		printf("\n Usage: %s \n", argv[0]);
        	printf("\n Usage: %s <ip of server> \n",argv[0]);
        	return 1;
    	} 
	else /* Otherwise, determine the IP based off argc */
	{
		/* If they don't provide an IP, use the default one */
		if (argc == 1)
			ip = DEF_IP;
		/* Otherwise, use the IP they provide */
		else
			ip = argv[1];
	}

    	memset(recvBuff, '0',sizeof(recvBuff));

	/* Create the Socket */
    	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    	{
        	printf("\n Error : Could not create socket \n");
        	return 1;
    	}

    	memset(&serv_addr, '0', sizeof(serv_addr)); 
    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_port = htons(DEF_PORT); 

	/* Convert the IPv4/IPv6 Address from text to binary */
    	if(inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
    	{
        	printf("\n inet_pton error occured\n");
        	return 1;
    	} 

	/* Connect to the server side socket */
    	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    	{
       		printf("\n Error : Connect Failed \n");
       		return 1;
    	} 

	/* Read from the server socket */
    	while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    	{
        	recvBuff[n] = 0;
        	if(fputs(recvBuff, stdout) == EOF)
        	{
            		printf("\n Error : Fputs error\n");
        	}
    	} 

	/* Output error from read */
    	if(n < 0)
    	{
        	printf("\n Read error \n");
    	} 
    	return 0;
}	
