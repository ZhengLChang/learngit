#Makefile
CC=gcc
INCLUDE=-I ../myinclude/
LIB=-lpthread
CFLAGS=-g -Wall -Werror -D_REENTRANT ${LIB} ${INCLUDE}
#$(INCLUDE)
MainFile=""
OutPut=$(patsubst %.c, %, ${MainFile})
src=$(wildcard ../mylib/*.c)
target=$(patsubst %.c, %.o, ${src} ${MainFile})
springcleaning=$(patsubst %.c, %, $(wildcard ./*.c))
springcleaning+=$(patsubst %.c, %.o, $(wildcard ./*.c))
springcleaning+=$(patsubst %.c, %.o, ${src})

.PHONY: all clean

all: $(OutPut)
#%.o: %.c
#	$(CC) ${CFLAGS} -c
$(OutPut):${target}
	$(CC) ${CFLAGS} ${INCLUDE} ${target}  -o $@

	
clean:
	-rm  ${springcleaning}
