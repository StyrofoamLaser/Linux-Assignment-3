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
CC_INSTRUCTIONS=-O1 $(WARNINGS) $(ISO) $(DEBUG)
CC_DRIVER= $(CC_INSTRUCTION) $(DEFINE)

INC=-I. -I/usr/include -I/usr/src/kernels/`uname -r`/include

DRIVER=mapdriver.o
MODULE=mapdriver.ko

FORKTEST_EXE=mapdriver-forkTest.exe
FORKTEST_OBJ=mapdriver-forkTest.o
FORKTEST_SRC=mapdriver-forkTest.c
FORKTEST_H=mapdriver-forkTest.h

TEST_EXE=mapdriver-test.exe
TEST_OBJ=mapdriver-test.o
TEST_SRC=mapdriver-test.c
TEST_H=mapdriver-test.h

SERVER_OBJ=mapserver.o
SERVER_SRC=mapserver.c
SERVER_H=mapserver.h
SERVER_EXE=mapserver.exe

CLIENT_OBJ=mapclient.o
CLIENT_SRC=mapclient.c
CLIENT_H=mapclient.h
CLIENT_EXE=mapclient.exe

DEPENDENCY=socket_common.c
DEPEND_OBJ=$(DEPENDENCY:.c=.o)

SRC=$(FORKTEST_SRC) $(TEST_SRC) $(SERVER_SRC) $(CLIENT_SRC)

H=$(SERVER_H) $(CLIENT_H) common.h socket_common.h

OBJ=$(SRC:.c=.o)  $(DRIVER)

EXE=$(SRC:.c=.exe)


DIR=~/dev/asciimap

obj-m += $(DRIVER)

all: depend compile
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	@echo "Map Driver has been compiled"
	@echo "Use 'make register' to register the module with the OS"
	@echo "Then follow the module's suggestion to 'mknod /dev/asciimap ' with the correct major number"
	@echo "To unregister run 'make unregister' or 'make clean-all'"
	@echo ""

.SUFFIXES: .exe

.c.o:
	$(CC) $(CC_INSTRUCTIONS) $(INC) -c $<

.o.exe:
	$(CC) $< -o $@

depend: $(SERVER_EXE) $(CLIENT_EXE)

$(SERVER_EXE): $(DEPEND_OBJ) $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) $(DEPEND_OBJ) -o $(SERVER_EXE)

$(CLIENT_EXE): $(DEPEND_OBJ) $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) $(DEPEND_OBJ) -o $(CLIENT_EXE)

compile: $(EXE) $(H)

register: $(DRIVER)
	insmod ./$(MODULE)
	modinfo $(MODULE)
	lsmod | grep mapdriver
	@echo ""
	@echo "Map Driver Device has been registered."
	@echo ""


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
	$(CC) $(CC_DRIVER) $(INC) -c mapdriver.c

clean:
	-rm -f $(EXE) $(OBJ) $(DEPEND_OBJ)
	-make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

clean-all: unregister clean

fork-test:  $(FORKTEST_EXE)
	./$(FORKTEST_EXE)

test: $(TEST_EXE)
	./$(TEST_EXE)

socket-test: $(SERVER_EXE) $(CLIENT_EXE)
	./$(SERVER_EXE)&
	ss -pa | grep $(SERVER_EXE)
	./$(CLIENT_EXE)

kill-server:
	pkill -f ./$(SERVER_EXE)
	
