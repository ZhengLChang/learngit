#Makefile
CC=gcc
INCLUDE=-I${MYGITPATH}/PublicLic/pj_project/pj_lib/include
LIB=-lpthread -lpj-i686-pc-linux-gnu -lfreetype  -ljpeg -lpng12 -ldl -lm -lrt
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
	$(CC) ${target}  -o $@ ${CFLAGS} ${INCLUDE} 
	
clean:
	-@rm  ${springcleaning}
