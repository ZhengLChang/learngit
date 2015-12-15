#Makefile
CC=gcc
INCLUDE=-I ../myinclude/
LIB=-lpthread
CFLAGS=-g -Wall -Werror ${LIB} ${INCLUDE}
#$(INCLUDE)
MainFile=""
OutPut=$(patsubst %.c, %, ${MainFile})
src=$(wildcard ../mylib/*.c)
target=$(patsubst %.c, %.o, ${src} ${MainFile})

.PHONY: all clean

all: $(OutPut)
#%.o: %.c
#	$(CC) ${CFLAGS} -c
$(OutPut):${target}
	$(CC) ${CFLAGS} ${INCLUDE} ${target}  -o $@

	
clean:
	-rm  ${target}
