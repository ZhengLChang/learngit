#Makefile

CFLAGS:=
CC:=gcc -g
CLIBS:=
INCLUDE:=-I ./
MainFile=main.c
Output=$(patsubst %.c, %, $(MainFile))
Source+=InsertionSort.c
Source+=Shellsort.c
Source+=Mergesort.c

all:$(Output)
.PHONY: all


$(Output):sort.h $(MainFile) 
	$(CC) $(Source) $(MainFile) $(CFLAGS) $(INCLUDE) -o $@

clean:
	-@rm -rf *.o
