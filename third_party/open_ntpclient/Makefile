# Under Solaris, you need to
#    CFLAGS += -xO2 -Xc
#    LDLIBS += -lnsl -lsocket
# Some versions of Linux may need
#    CFLAGS += -D_GNU_SOURCE
# To cross-compile
#    CC = arm-linux-gcc
#CC=/opt/brcm/hndtools-mipsel-linux/bin/mipsel-linux-gcc
# To check for lint
# -Wundef not recognized by gcc-2.7.2.3
CFLAGS += -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wshadow \
 -Waggregate-return -Wnested-externs -Winline -Wwrite-strings \
 -Wstrict-prototypes

CFLAGS += -O

CFLAGS += $(IFX_CFLAGS)
LDFLAGS += $(IFX_LDFLAGS)

all: ntpclient

test: ntpclient
	./ntpclient -d -r <test.dat

#ntpclient: ntpclient.o phaselock.o resolv.o
ntpclient: ntpclient.o phaselock.o
	$(CC) $(CFLAGS) $^ -o $@

adjtimex: adjtimex.o

distclean: clean

clean:
	rm -f ntpclient adjtimex *.o
