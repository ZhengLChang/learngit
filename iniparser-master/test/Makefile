#
# iniparser tests Makefile
#

CC      = gcc
CFLAGS  = -g -I../src
LFLAGS  = -L.. -liniparser
AR	    = ar
ARFLAGS = rcv
RM      = rm -f
MainFile=""
OutPut=$(patsubst %.c, %, ${MainFile})


default: all

all: iniexample parse ${OutPut}

iniexample: iniexample.c
	$(CC) $(CFLAGS) -o iniexample iniexample.c -I../src -L.. -liniparser

parse: parse.c
	$(CC) $(CFLAGS) -o parse parse.c -I../src -L.. -liniparser
${OutPut}: $(MainFile)
	$(CC) $(CFLAGS) -o $(OutPut) $(MainFile) -I../src -L.. -liniparser


clean veryclean:
	$(RM) iniexample example.ini parse



