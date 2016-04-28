#Makefile
CC=gcc
INCLUDE=-I${MYGITPATH}/PublicLic/pj_project/pj_lib/include -I /usr/include/dbus-1.0 -I /usr/lib/dbus-1.0/include -ldbus-1 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
LIB=-lpthread -lpj-i686-pc-linux-gnu -lfreetype  -ljpeg -lpng12 -ldl -lm -lrt -ldbus-1
CFLAGS=-g -Wall -Werror -D_REENTRANT -rdynamic  ${LIB} ${INCLUDE}
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
