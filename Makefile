INCLUDE_DIR := ./inc
SRC_PATH := ./src
CFLAG = -I${INCLUDE_DIR} -Wall -O2
CC = /usr/bin/gcc
LDLIBS = -lm -lpthread

vpath %.c ${SRC_PATH}

.PHONY: all clean install
ALL = server client

all: ${ALL}
	cscope -Rb

clean:
	rm -rf ${ALL} *.o 
	clear

install:
	cp ${ALL} ./bin/

%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

server: server.o
	${CC} ${CFLAG} $^ -o $@

client: client.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

