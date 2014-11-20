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
	char read_buf[BSIZE];
	char write_buf[BSIZE];
	int fd, n, i, m;

	if((fd = open("/dev/asciimap", O_RDWR)) >= 0)
	{
	
		for(i = 0; i < (BSIZE);i++)
		{
			if(i % 50 == 0)
				write_buf[i] = '\n';
			else
				write_buf[i] = '0';
		}

		m = write(fd, write_buf, BSIZE);
		printf("Written: %i\nNow I will write the remaining lines\n", m);

		lseek(fd, 0, SEEK_SET);

		do
		{
			/*Read in from the driver to the buffer*/
			n = read(fd, read_buf, BSIZE);

			/*Print what was written if anything was written*/
			printf(read_buf);

			for(i = 0; i < n; i++)
			{
				read_buf[i] = '\0';
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
