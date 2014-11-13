# Makefile
# 
# Makefile to build our mapdriver, run test programs and register the driver with the OS
#
# Cody Carlson - Nov 13, 2014
####

CC= gcc
CC_INSTRUCTIONS=-g -Wall

EXE=mapdriver-test.exe	#
SRC=mapdriver-test.c	#these do not exist yet, but I put some placeholders in for them for later
DRIVER=mapdriver.c
MODULE=mapdriver.ko

all:
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

$(DRIVER): mapdriver.c
	$(CC) $(CFLAGS) -c mapdriver.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

clean-all:
	unregister
