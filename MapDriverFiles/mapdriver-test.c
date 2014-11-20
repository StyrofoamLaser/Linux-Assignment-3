#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"

int main(argc, argv)
	int argc;
	char* argv[];
{
	char buf[BSIZE];
	int fd, n, i;

	if((fd = open("/dev/asciimap", O_RDWR)) >= 0)
	{
	
		do
		{
			/*Read in from the driver to the buffer*/
			n = read(fd, buf, BSIZE);

			/*Print what was written if anything was written*/
			printf(buf);

			for(i = 0; i < n; i++)
			{
				buf[i] = '\0';
			}
			
		}
		while (n > 0);

		close(fd);
	}
	else
	{
		perror("open(/dev/asciimap) failed");
		exit(1);
	}

    exit(0);
}

/* EOF */
