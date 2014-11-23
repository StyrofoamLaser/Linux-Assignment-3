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

FORKTEST_EXE=mapdriver-forkTest.exe
FORKTEST_OBJ=mapdriver-forkTest.o $(DRIVER)
FORKTEST_SRC=mapdriver-forkTest.c
FORKTEST_H=mapdriver-forkTest.h

TEST_EXE=mapdriver-test.exe
TEST_OBJ=mapdriver-test.o $(DRIVER)
TEST_SRC=mapdriver-test.c
TEST_H=mapdriver-test.h

SERVER_OBJ=mapserver.o $(DRIVER)
SERVER_SRC=mapserver.c
SERVER_H=mapserver.h
SERVER_EXE=mapserver.exe

CLIENT_OBJ=mapclient.o $(DRIVER)
CLIENT_SRC=mapclient.c
CLIENT_H=mapclient.h
CLIENT_EXE=mapclient.exe

DIR=~/dev/asciimap

obj-m += $(DRIVER)

all: test-exes
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	@echo "Map Driver has been compiled"
	@echo "Use 'make register' to register the module with the OS"
	@echo "Then follow the module's suggestion to 'mknod /dev/asciimap ' with the correct major number"
	@echo "To unregister run 'make unregister' or 'make clean-all'"
	@echo ""

compile: EXEs $(OBJ)

register: $(DRIVER)
	insmod ./$(MODULE)
	modinfo $(MODULE)
	lsmod | grep mapdriver
	@echo ""
	@echo "Map Driver Device has been registered."
	@echo ""

test-exes: $(FORKTEST_OBJ) $(TEST_OBJ)
	$(CC) $(FORKTEST_OBJ) -o  $(FORKTEST_EXE)
	$(CC) $(TEST_OBJ) -o $(TEST_EXE) 

socket-exes: $(SERVER_OBJ) $(CLIENT_OBJ)
	$(CC) $(SERVER_OBJ) -o $(SERVER_EXE)
	$(CC) $(CLIENT_OBJ) -o $(CLIENT_EXE)

$(FORKTEST_OBJ): $(FORKTEST_SRC) common.h
	$(CC) $(CC_INSTRUCTIONS) $(INC) -c $(FORKTEST_SRC)

$(TEST_OBJ): $(TEST_SRC) common.h
	$(CC) $(CC_INSTRUCTIONS) $(INC) -c $(TEST_SRC)

$(SERVER_OBJ): $(SERVER_SRC) common.h
	$(CC) $(CC_INSTRUCTIONS) $(INC) -c $(SERVER_SRC)

$(CLIENT_OBJ): $(CLIENT_SRC) common.h
	$(CC) $(CC_INSTRUCTIONS) $(INC) -c $(CLIENT_SRC)

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

fork-test:  test-exes
	./$(FORKTEST_EXE)

test: test-exes
	./$(TEST_EXE)

socket-test: socket-exes
	./$(SERVER_EXE)&
	./$(CLIENT_EXE)
	
