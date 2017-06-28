#Makefile
CC=gcc -std=c99
INCLUDE=-I../libmxml-0.9.1/include
LIB=-lpthread 
CFLAGS=-g -Wall -Werror -D_REENTRANT ${LIB} ${INCLUDE}
#$(INCLUDE)
OutPut=main
src=
target=$(patsubst %.c, %.o, strings_tool/src/*)
springcleaning=$(patsubst %.c, %, $(wildcard ./*.c))
springcleaning+=$(patsubst %.c, %.o, $(wildcard ./*.c))
springcleaning+=$(patsubst %.c, %.o, ${src})

.PHONY: all clean

all: $(OutPut)
$(OutPut):${target} 
	$(CC) ${CFLAGS} ${INCLUDE} ${target}  ./libmxml-0.9.1/lib/*.o -o $@

	
clean:
	-rm  ${springcleaning}
