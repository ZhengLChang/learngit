#Makefile
CC=gcc
INCLUDE=
LIB=-lpthread
CFLAGS=-g -Wall -Werror -D_REENTRANT ${LIB} ${INCLUDE}
MainFile=""
OutPut=$(patsubst %.c, %, ${MainFile})
src=
target=$(patsubst %.c, %.o, ${MainFile})
springcleaning=$(patsubst %.c, %, $(wildcard ./*.c))
springcleaning+=$(patsubst %.c, %.o, $(wildcard ./*.c))
springcleaning+=$(patsubst %.c, %.o, ${src})

.PHONY: all clean

all: $(OutPut)
$(OutPut):${target}
	$(CC) ${CFLAGS} ${INCLUDE} ${target}  -o $@

	
clean:
	-@rm  ${springcleaning}
	-@cd ./cut_rod && $(MAKE) clean
