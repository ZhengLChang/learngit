DESTDIR = $(INSTROOT)/usr/local
BINDIR = $(DESTDIR)/sbin
MANDIR = $(DESTDIR)/share/man/man8

CFLAGS= -O -I../include/net
OBJS = pppdump.o bsd-comp.o deflate.o zlib.o

INSTALL= install

all:	pppdump

pppdump: $(OBJS)
	$(CC) -o pppdump $(OBJS)

clean:
	rm -f pppdump $(OBJS) *~

install:
	mkdir -p $(BINDIR) $(MANDIR)
	$(INSTALL) -s -c pppdump $(BINDIR)
	$(INSTALL) -c -m 444 pppdump.8 $(MANDIR)
