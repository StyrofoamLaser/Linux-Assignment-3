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

#include <linux/fs.h>

#include <asm/uaccess.h>

/* Return codes */
#define SUCCESS 0

/* Device Declarations ************/


#define DRV_BUF_SIZE 50

/* Device name */
#define DEVICE_NAME "/dev/asciimap"


/* Driver status structure */
typedef struct _driver_status
{
	/* The next character to be output */
	char cur_char;

	/* The current length of the buffer */
	int buf_size;

	/* Prevents corcurent access to the device */
	bool busy;

	char buf[DRV_BUF_SIZE];

	/* Pointer to the current
 	 * place in the buffer 
 	 */ 
	char* buf_ptr;

	/* The major device numer */
	int major;

	/* The minor device number */
	int minor;

} driver_status_t;

/* Driver functions' prototypes */
static int device_open( struct inode*, struct file* );
static int device_release( struct inode*, struct file* );
static ssize_t device_read( struct file*, char*, size_t, loff_t* );
static ssize_t device_write( struct file*, const char*, size_t, loff_t* );

/* Kernel module-related */

/* Holds functions called when a process does
 * something to the device.
 */

struct file_operations Fops =
{
	NULL, /* owner */
	NULL, /* seek */
	device_read,
	device_write,
	NULL, /* readdir */
	NULL, /* poll/select */
	NULL, /* ioctl */
	NULL, /* mmap */
	device_open,
	NULL, /* flush */
	device_release /* close */
};

int init_module(void);
void cleanup_module(void);

#endif /* _MAPDRIVER_H */
