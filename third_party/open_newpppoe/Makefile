# PPP top-level Makefile for Linux.

DESTDIR = $(INSTROOT)/usr/local
BINDIR = $(DESTDIR)/sbin
INCDIR = $(DESTDIR)/include
MANDIR = $(DESTDIR)/share/man
ETCDIR = $(INSTROOT)/etc/ppp

# uid 0 = root
INSTALL= install

all:
	cd pppd/plugins; $(MAKE) $(MFLAGS) all
	cd pppd; $(MAKE) $(MFLAGS) all

install: $(BINDIR) $(MANDIR)/man8 install-progs install-devel

install-progs:
	cd pppd/plugins; $(MAKE) $(MFLAGS) install
	cd pppd; $(MAKE) $(MFLAGS) install

install-etcppp: $(ETCDIR) $(ETCDIR)/pap-secrets \
	$(ETCDIR)/chap-secrets

install-devel:
	cd pppd; $(MAKE) $(MFLAGS) install-devel

$(ETCDIR)/options:
$(ETCDIR)/pap-secrets:
	$(INSTALL) -c -m 600 etc.ppp/pap-secrets $@
$(ETCDIR)/chap-secrets:
	$(INSTALL) -c -m 600 etc.ppp/chap-secrets $@

$(BINDIR):
	$(INSTALL) -d -m 755 $@
$(MANDIR)/man8:
	$(INSTALL) -d -m 755 $@
$(ETCDIR):
	$(INSTALL) -d -m 755 $@

clean:
	rm -f `find . -name '*.[oas]' -print`
	rm -f `find . -name 'core' -print`
	rm -f `find . -name '*~' -print`
	cd chat; $(MAKE) clean
	cd pppd/plugins; $(MAKE) clean
	cd pppd; $(MAKE) clean
	cd pppstats; $(MAKE) clean
	cd pppdump; $(MAKE) clean

dist-clean:	clean
	rm -f Makefile `find . -name Makefile -print`

#kernel:
#	cd linux; ./kinstall.sh

# no tests yet, one day...
installcheck:
	true
