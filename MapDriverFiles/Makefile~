# Makefile
# 
# Makefile to build our mapdriver, run test programs and register the driver with the OS
#
# Cody Carlson - Nov 13, 2014
####

CC=gcc
DEBUG=-g -D_DEBUG
DEFINE=-DMODULE -D__KERNEL__ -DLINUX
WARNINGS=-Wall -Wmissing-prototypes -Wmissing-declarations
#ISO=-ansi -pedantic
CC_INSTRUCTIONS=-O1 $(WARNINGS) $(ISO) $(DEBUG) $(DEFINE)

INC=-I. -I/usr/include -I/usr/src/kernels/`uname -r`/include

EXE=mapdriver-test.exe	#
SRC=mapdriver-test.c	#these do not exist yet, but I put some placeholders in for them for later
DRIVER=mapdriver.o
MODULE=mapdriver.ko

obj-m += $(DRIVER)

all: $(DRIVER)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	@echo "Map Driver has been compiled"
	@echo "Use 'make register' to register the module with the OS"
	@echo "Then follow the module's suggestion to 'mknod /dev/asciimap ' with the correct major number"
	@echo "To unregister run 'make unregister' or 'make clean-all'"
	@echo ""

register: $(DRIVER)
	insmod ./$(MODULE)
	modinfo $(MODULE)
	lsmod | grep mapdriver
	@echo ""
	@echo "Map Driver Device has been registered."
	@echo ""

unregister: register
	rmmod mapdriver

$(DRIVER): mapdriver.h mapdriver.c
	$(CC) $(CFLAGS) $(INC) -c mapdriver.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

clean-all: unregister clean
	
