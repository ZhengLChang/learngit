#Makefile
CC=gcc
INCLUDE=
LIB=-lpthread
CFLAGS=-g -Wall -Werror -D_REENTRANT ${LIB} ${INCLUDE}
#$(INCLUDE)
MainFile=""
OutPut=$(patsubst %.cpp, %, ${MainFile})
src=
target=$(patsubst %.cpp, %.o, ${MainFile})
springcleaning=$(patsubst %.cpp, %, $(wildcard ./*.c))
springcleaning+=$(patsubst %.cpp, %.o, $(wildcard ./*.c))
springcleaning+=$(patsubst %.cpp, %.o, ${src})

.PHONY: all clean

all: $(OutPut)
$(OutPut):${target}
	$(CC) ${CFLAGS} ${INCLUDE} ${target}  -o $@

	
clean:
	-@rm  ${springcleaning}
	-@cd ./cut_rod && $(MAKE) clean
