#include "mapdriver.h"

static driver_status_t status =
{
	'0',
	 0,
	 0,
	 0,
	false,
	{0},
	{0},
	NULL,
	-1,
	-1
};

/*Called when a process tries to open the
 * device file.
 */
static int device_open(inode, file)
	struct inode* inode;
	struct file* file;
{


#ifdef _DEBUG
	printk("device_open(%p,%p)\n", inode, file);
#endif

	/*Get the minor device*/
	status.minor = inode->i_rdev >> 8;
	status.minor = inode->i_rdev & 0xFF;

	printk
	(
		"Device: %d.%d, busy: %d\n",
		status.major,
		status.minor,
		status.busy
	);

	/* Does nothing if already talking to a process */
	if(status.busy)
		return -EBUSY;
	
	/* Now talking to a process */
	status.busy = true;

	return SUCCESS;
}

/*Function called when the process
 * closes the device file.
 */
static int device_release(inode, file)
	struct inode* inode;
	struct file* file;
{
#ifdef _DEBUG
	printk("device_released(%p,%p)\n", inode, file);
#endif

	status.busy = false;
	return SUCCESS;	
}

/* Reset the map back to original */
static int device_ioctl( int d, int request, ...)
{
	return SUCCESS;
}

/*Called when a process that has opened the device file
 * attempts to read from the file.
 */
static ssize_t device_read(file, buffer, length, offset)
	struct file*	    file;
   	       char*      buffer;
   	      size_t      length;
             loff_t*      offset;
{
	/*The number of bytes read*/
	int bytes_read = 0;
	
	/*Reads through till it reaches length or null terminator*/
	while(length > 0 && *status.buf_ptr != '\0')
	{
		put_user(*status.buf_ptr, buffer++);
		length--;
		status.buf_ptr++;
		bytes_read++;
	}

	#ifdef _DEBUG
	printk
	(
		"mapdriver::device_read() - Read %d bytes, %d left\n",
		bytes_read;
		length;
	);
	#endif

	/*Return the bytes read*/
	return bytes_read;
}

/* Called when a process tries to write to the device file. */
static ssize_t device_write(file, buffer, length, offset)
	struct file*	file;
	const char* 	buffer;
	size_t	    	length;
	loff_t*		offset;
{
	int bytes_written = 0;
	int total_length = length;
	while(length > 0 && status.cur_buf_length < BSIZE_SQUARED)
	{
		if(length < 0 ||
		   status.cur_buf_length >= BSIZE_SQUARED)
		{
			/*Print error and break*/
			printk("ERROR: Attempted to write outside the map buffer");
			break;
		}
		
		/*Increment values*/
		*status.buf_ptr = buffer[total_length - length];
		length--;
		bytes_written++;
		status.buf_ptr++;
		
		/*If new pointer is null char increment length*/
		if(*status.buf_ptr == '\0')
			status.cur_buf_length++;
	} 

	#ifdef _DEBUG
	printk
	(
		"mapdriver::device_write() - Length: [%d], BufL [%s]\n",
		length,
		buffer
	);
	#endif

	/*Returns the bytes written*/
	return bytes_written;
}


	/* Rewind back to '0' */
/*	status.curr_char = '0';

	return nbytes;
*/

static off_t device_lseek(int fd, off_t offset, int whence)
{
	/* Defines where in the buffer to begin from. This is defined by whence */
	int bufferIndex = 0;

	switch(whence)
	{
		/* Begin from the start of the buffer */
		case SEEK_SET:
			bufferIndex = 0;
			break;
		/* Begin from the current buffer pointer position */
		case SEEK_CUR:
			bufferIndex = status.buf_ptr - status.b_size_buf;
			break;
		/* Begin from the end of the buffer */
		case SEEK_END:
			bufferIndex = BSIZE_SQUARED - 1;
			break;
	}

	/* Perform bounds checking with the index first before moving the pointer */
	bufferIndex += offset;

	/* If the buffer index is out of the map bounds, we return -1 and set errno */
	if (bufferIndex < 0 ||
	    bufferIndex >= BSIZE_SQUARED)
	{
		/* errno = EINVAL; */
		printk
		(
			"ERROR: lseek - Offset is out of bounds."
		);

		return -1;
	}

	/* Setup the buffer pointer based off the starting bufferIndex */
	status.buf_ptr = status.b_size_buf + bufferIndex;

	status.buf_ptr += offset;

	return (off_t)(sizeof(char) * bufferIndex);
}

/* Initialize the module - Register the device */
int init_module(void)
{
	int i = 0;
	char initials_array[] = {'R', 'H', 'V', 'L', 'J', 'P', 'C', 'C'};
	int initials_array_length = 8;
	status.major = register_chrdev
	(
		0,
		DEVICE_NAME,
		&Fops
	);

	if(status.major < 0)
	{
		printk
		(
			"Registering device failed with %d\n",
			status.major
		);
		return status.major;
	}

	printk
	(
		"Registration successful, major device number is %d.\n",
		status.major
	);

	printk("Suggested device file:\n\nmknod %s c %d 0\n\n",	DEVICE_NAME, status.major);

	
	/*Starting width and height taking into account for the '\n' in width*/
	status.cur_width = START_SIZE + 1;
	status.cur_height = START_SIZE;

	for( i = 0; i < TOTAL_STATIC_BUF_LENGTH; i++)
	{
		if((i + 1) % status.cur_width - 1 != 0)
		{
			status.static_buf[i] = initials_array[i % initials_array_length];
		}
		else
		{
			status.static_buf[i] = '\n';
		}
		
		status.b_size_buf[i] = status.static_buf[i];

	}

	status.static_buf[TOTAL_STATIC_BUF_LENGTH - 1] = '\0';

	for( i = TOTAL_STATIC_BUF_LENGTH; i < BSIZE_SQUARED; i++)
	{
		status.b_size_buf[i] = '\0';
	}
	
	status.cur_buf_length = TOTAL_STATIC_BUF_LENGTH;
	status.buf_ptr = status.b_size_buf;		
	

	return SUCCESS;
}

/* Unregisters the appropriate files */
void cleanup_module(void)
{
	unregister_chrdev(status.major, DEVICE_NAME);
}

/* EOF */
