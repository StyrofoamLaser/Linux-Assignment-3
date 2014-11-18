#include "mapdriver.h"

static driver_status_t status =
{
	'0',
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
	struct file*	   file;
   	       char*     buffer;
   	      size_t     length;
             loff_t*     offset;
{
	/*Return the bytes read*/
	return 0;
}
/*	int bytes_read = 0;

	while(length > 0)
	{
		put_user(status.curr_char, buffer);

		length--;
		bytes_read++;
	}

#ifdef _DEBUG
	printk
	(	"mapdriver::device_read() - Read %d bytes, %d left\n",
		bytes_read,
		length
	);
#endif

	if(++status.curr_char == 127)
		status.curr_char = '0';

	return bytes_read;
}
*/
/* Called when a process tries to write to the device file. */
static ssize_t device_write(file, buffer, length, offset)
	struct file*	file;
	const char* 	buffer;
	size_t	    	length;
	loff_t*		offset;
{
	/*Return the bytes written*/
	return 0;
}

/*The old write function for reference*/
/*	int nbytes = 0;
#ifdef _DEBUG
	printk
	(
		"mapdriver::device_write() - Length: [%d], BufL [%s]\n",
		length,
		buffer
	);
#endif
*/
	/* Rewind back to '0' */
/*	status.curr_char = '0';

	return nbytes;
*/

static off_t device_lseek(int fd, off_t offset, int whence)
{
	return -1;
}

/* Initialize the module - Register the device */
int init_module(void)
{

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


	printk
	(
		"Suggested device file:\n\n" \
		"mknod %s c %d <minor>\n\n" \
		DEVICE_NAME,
		status.major
	);


	for(i = 0; i < TOTAL_STATIC_BUF_LENGTH - 1; i++)
	{
		status.staticBuf[i] = '0';
	}


	return SUCCESS;
}

/* Unregisters the appropriate files */
void cleanup_module(void)
{
	unregister_chrdev(status.major, DEVICE_NAME);
}

/* EOF */
