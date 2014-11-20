/*
* mapdriver.h
* Header file for mapdriver.c
* Cody Carlson - Nov 13, 2014
* Content Added - Robert Hacker - Nove 16, 2014
*/

#ifndef _MAPDRIVER_H
#define _MAPDRIVER_H

/* Required for kernel modules */
#include <linux/kernel.h>
#include <linux/module.h>

/* Required for character devices */
#include <linux/fs.h>
#include <asm/uaccess.h>

/* Return codes */
#define SUCCESS 0

/* Device Declarations ************/

/*The length and height for the buffer*/
#define START_SIZE 50

/* A 50 x 50 square with an extra 50 for the '\n' and 1 for the '\0'*/
#define TOTAL_STATIC_BUF_LENGTH 2551

/*The maximum length and height for the buffer*/
#define BSIZE 75

/* A 75 x 75 square with an extra 75 for the '\n' and 1 for the '\0'*/
#define BSIZE_SQUARED 5701

/* Device name */
#define DEVICE_NAME "/dev/asciimap"


/* Driver status structure */
typedef struct _driver_status
{
	/* The next character to be output */
	char curr_char;

	/* The current length of the buffer */
	int cur_buf_length;

	/* The current width of the map */
	int cur_width;

	/* The current height of the buffer */
	int cur_height;

	/* Prevents corcurent access to the device */
	bool busy;

	/* Static buffer holding initial map 50x50  */
	char static_buf[TOTAL_STATIC_BUF_LENGTH];

	/* Buffer that is initially set to the staticBuf */
	char b_size_buf[BSIZE_SQUARED];

	/* Pointer to the current poisiont in the buffer */ 
	char* buf_ptr;

	/* The major device number */
	int major;

	/* The minor device number */
	int minor;

} driver_status_t;

/* Driver functions' prototypes */
static int device_open( struct inode*, struct file* );
static int device_release( struct inode*, struct file* );
static int device_ioctl( int d, int request, ...); 
static ssize_t device_read( struct file*, char*, size_t, loff_t* );
static ssize_t device_write( struct file*, const char*, size_t, loff_t* );
static off_t device_lseek( int fd, off_t offset, int whence );

/* Kernel module-related */

/* Holds functions called when a process does
 * something to the device.
 */

struct file_operations Fops =
{
	NULL, /* owner */
	device_lseek, /* seek */
	device_read,
	device_write,
	NULL, /* readdir */
	NULL, /* poll/select */
	device_ioctl, 
	NULL, /* mmap */
	device_open,
	NULL, /* flush */
	device_release /* close */
};

int init_module(void);
void cleanup_module(void);

#endif /* _MAPDRIVER_H */
