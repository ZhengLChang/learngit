#Makefile
CC=g++
INCLUDE=
LIB=-lpthread
CFLAGS=-g -Wall -Werror -D_REENTRANT ${LIB} ${INCLUDE}
#$(INCLUDE)
MainFile=""
OutPut=$(patsubst %.cpp, %, ${MainFile})
src=
target=$(patsubst %.cpp, %.o, ${MainFile})
springcleaning=$(patsubst %.cpp, %, $(wildcard ./*.cpp))
springcleaning+=$(patsubst %.cpp, %.o, $(wildcard ./*.cpp))
springcleaning+=$(patsubst %.cpp, %.o, ${src})

.PHONY: all clean

all: $(OutPut)
$(OutPut):${target}
	$(CC) ${CFLAGS} ${INCLUDE} ${target}  -o $@

	
clean:
	-@rm  ${springcleaning} a.out
