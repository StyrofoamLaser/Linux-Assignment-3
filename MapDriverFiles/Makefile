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
#ISO=-ansi
CC_INSTRUCTIONS=-O1 $(WARNINGS) $(ISO) $(DEBUG) $(DEFINE)

INC=-I. -I/usr/include -I/usr/src/kernels/`uname -r`/include

DRIVER=mapdriver.o
MODULE=mapdriver.ko
EXE=mapdriver-test.exe
OBJ=mapdriver-test.o $(DRIVER)	
DIR=~/dev/asciimap

obj-m += $(DRIVER)

all: $(EXE)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	@echo "Map Driver has been compiled"
	@echo "Use 'make register' to register the module with the OS"
	@echo "Then follow the module's suggestion to 'mknod /dev/asciimap ' with the correct major number"
	@echo "To unregister run 'make unregister' or 'make clean-all'"
	@echo ""

compile: $(EXE) $(OBJ)

register: $(DRIVER)
	insmod ./$(MODULE)
	modinfo $(MODULE)
	lsmod | grep mapdriver
	@echo ""
	@echo "Map Driver Device has been registered."
	@echo ""

$(EXE): mapdriver-test.o
	$(CC) mapdriver-test.o -o  $(EXE) 

mapdriver-test.o: mapdriver-test.c common.h
		$(CC) $(CC_INSTRUCTIONS) $(INC) -c mapdriver-test.c

setup:
ifeq (, $(wildcard $(DIR)))
	eval `dmesg | grep mknod | tail -1`
endif	

build:
	make all
	make register
	make setup

unregister: 
	rmmod mapdriver

$(DRIVER): mapdriver.h mapdriver.c
	$(CC) $(CC_INSTRUCTIONS) $(INC) -c mapdriver.c

clean:
	rm -f $(EXE) $(OBJ)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

clean-all: unregister clean
	
