#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>


#include "common.h"

int HEIGHT = 20;
int WIDTH = 20;

struct option longopts[] = {
{ "height", required_argument, NULL, 'h'},
{ "width", required_argument, NULL, 'w'},
{ 0,0,0,0}
};

void carveText(char* file)
{
	char* buff;
	char singleChar[1];
	int eof = 1;
	int fid;
	int i = 0;
	int shouldPutInBlank = 0;
	
	buff = malloc((WIDTH+1) * (HEIGHT)  * sizeof(char));
	
	fid = open(file, O_RDONLY);
	
	if(fid != -1)
	{
		for(i = 0; i < (HEIGHT) * (WIDTH+1); i++)
		{
			if((i+1) % (WIDTH+1) == 0 && i != 0)
			{
				buff[i] = '\n';
				if(shouldPutInBlank == 1)
				{
					shouldPutInBlank = 0;
				}
				else
				{
					/*get to the next line in the file*/
					while(singleChar[0] != '\n' && eof != 0)
					{
						eof = read(fid, singleChar, sizeof(singleChar));
					}
				}
			}
			else if(eof == 0)
			{
				buff[i] = ' ';
			}
			else
			{
				if(shouldPutInBlank == 0)
				{
					eof = read(fid, singleChar, sizeof(char));
					if(eof == 0)
					{
						singleChar[0] = ' ';
					}
					buff[i] = singleChar[0];
				}
				else
				{
					buff[i] = ' ';
				}
				if(buff[i] == '\n')
				{
					shouldPutInBlank = 1;
					buff[i] = ' ';
				}
			}
		}
		printf(buff);
		
	}
	else
	{
		perror("Could not open file");
	}

	free(buff);	
}


int main(argc, argv)
	int argc;
	char* argv[];
{
	/*char read_buf[BSIZE];*/
	/*char write_buf[BSIZE];*/
	int i;
	
	char* fileName = "genmap.sh";
	char* newargv[] = {fileName, NULL};
	char* newenvp[] = {"HOME=/usr/home", "LOGNAME=home", NULL};

	/*
         * Parent keeps a dynamic array of PIDs of all its children
         * to wait for them afterwards.
         */
	pid_t* piChildrenPIDs = NULL;

	/*
         * Next child's PID              
         */
        pid_t iChildPID;

        /*
         * Argument pointer the parent wishes to pass on to its next child
         */
        char* pcChildArgument = NULL;
	
	int c;

	while((c = getopt_long(argc, argv, "h:w:", longopts, NULL)) != -1)
	{
		switch(c)
		{
			case 'w':
				WIDTH = atoi(optarg);
				break;
			case 'h':
				HEIGHT = atoi(optarg);
				break;
			case 0:
				break;
			case':':
				perror("not an option");
				break;
			case'?':
				perror("not an option");
				break;
			default:
				perror("not an option");
				break;
		}
	}


	if(/*(fd = open("/dev/asciimap", O_RDWR)) >= 0*/1)
	{
		if(argc > 1)
		{
			/*
 			 * Allocate as much storage as many arguments are there
                	 */
        		piChildrenPIDs = (pid_t*)malloc(sizeof(pid_t) * (argc - 1));

			for(i = 1; i < argc; i++)
			{
		                /*
		                 * Update the pointer to the next argument so that child inherits it.
                                 * Every child will inherit a different pointer
                                 */
				pcChildArgument = argv[i];
				iChildPID = fork();


				if(iChildPID == 0)
				{
					/*stuff the child does*/
					if(pcChildArgument[0] != '-')
					{
						carveText(pcChildArgument);
					}
					/*child terminates*/
					exit(0);
				}
				else if(iChildPID > 0)
				{
					/*stuff the parent does*/
                        		/* store PID of the just forked child */
                        		piChildrenPIDs[i] = iChildPID;
				}
				else
				{
					/*fork failed*/
					perror("Fork failed!");
				}
			}
        		for(i = 1; i < argc; i++)
       			{
				/*Wait for all the children to finish*/
                		waitpid(piChildrenPIDs[i], NULL, 0);
                    	}
		}
		else
		{
			iChildPID = fork();
			
			if(iChildPID == 0)
			{
				execve(fileName, newargv, newenvp);
				perror("Exec failed!");/*exec failed*/
			}
			else if(iChildPID > 0)
			{
				/*wait for child*/
				waitpid(iChildPID, NULL, 0);
			}
			else
			{
				/*fork failed*/
				perror("Fork 2 failed");
			}
			
		}

/*		n = write(fd, write_buf, BSIZE);
		printf("Written: %i\nNow I will write the remaining lines\n", n);
		do
		{*/
			/*Read in from the driver to the buffer*/
		/*	n = read(fd, read_buf, BSIZE);*/

			/*Print what was written if anything was written*/
		/*	printf(read_buf);

			for(i = 0; i < n; i++)
			{
				read_buf[i] = '\0';
			}
			
		}
		while (n > 0);*/

		/*close(fd);*/
	}
	else
	{
		perror("open(/dev/asciimap) failed");
		exit(1);
	}

    exit(0);
}

/* EOF */
