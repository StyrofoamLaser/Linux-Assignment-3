#include "mapdriver.h"

static driver_status_t status =
{
	'0',
	false,
	{0},
	NULL,
	-1,
	-1
};

static int device_open(inode, file)
	struct inode* inode;
	struct file* file;
{
	static int counter = 0;

#ifdef _DEBUG
	printk("device_open(%p,%p)\n", inode, file);
#endif

	status.minor = inode->i_rdev >> 8;
	status.minor = inode->i_rdev & 0xFF;

	printk
	(
		"Device: %d.%d, busy: %d\n",
		status.major,
		status.minor,
		status.busy
	);

	if(status.busy)
		return -EBUSY;

	status.busy = true;

	sprintf
	(
		status.buf,
		"Message initalized\n"
	);

	status.buf_ptr = status.buf;

	return SUCCESS;
}

static ssize_t device_read(file, buffer, length, offset)
	struct file* file;
    char*	buffer;
    size_t	length;
    loff_t*	offset;
{
	int bytes_read = 0;

	while(length > 0)
	{
		put_user(status.curr_char, buffer);

		length--;
		bytes_read++;
	}

#ifdef _DEBUG
	printk
	(	"mapdriver:device_read() - Read %d bytes, %d left\n",
		bytes_read,
		length
	);
#endif

	/* Rewind back to '0' */
	status.curr_char = '0';

	return nbytes;
}

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

	return SUCCESS;
}

void cleanup_module(void)
{
	unregister_chrdev(status.major, DEVICE_NAME);
}

/* EOF */
