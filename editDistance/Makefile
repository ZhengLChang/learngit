#Makefile

CFLAGS:=
CC:=gcc -g
CLIBS:=
INCLUDE:=-I ./
MainFile=
Output=$(patsubst %.c, %, $(MainFile))

all:$(Output)
.PHONY: all


$(Output):editDistance.h editDistance.c $(MainFile) 
	$(CC) editDistance.c $(MainFile) $(CFLAGS) $(INCLUDE) -o $@

clean:
	-@rm -rf *.o
