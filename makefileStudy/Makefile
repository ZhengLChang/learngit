#Makefile
CC=gcc
INCLUDE=
LIB=-lpthread
CFLAGS=-g -Wall -Werror -D_REENTRANT ${LIB} ${INCLUDE}

HELLOWORLD := ./helloworld/
.PHONY: clean

$(HELLOWORLD)/helloworld:$(HELLOWORLD)/Makefile $(HELLOWORLD)/helloworld.c
	-@echo $(@D)
	-@echo $(^D)
	cd $(@D) && $(MAKE)
clean:
	-cd $(HELLOWORLD) && $(MAKE) clean
	-rm  ${springcleaning}
