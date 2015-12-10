#Makefile

CFLAGS:=
CC:=gcc
CLIBS:=
INCLUDE:=-I ./
MainFile=
Output=$(patsubst %.c, %, $(MainFile))

all:$(Output)
.PHONY: all


$(Output):MaxSubsequenceSum.h $(MainFile) 
	$(CC) $(MainFile) $(CFLAGS) $(INCLUDE) -o $@

clean:
	-@rm -rf *.o
