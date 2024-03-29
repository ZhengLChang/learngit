# Makefile.  Generated from Makefile.in by configure.
#***********************************************************************
#
# Makefile
#
# Makefile for Roaring Penguin's Linux user-space PPPoE client.
#
# Copyright (C) 2000 Roaring Penguin Software Inc.
#
# This program may be distributed according to the terms of the GNU
# General Public License, version 2 or (at your option) any later version.
#
# LIC: GPL
#
# $Id$
#***********************************************************************

# Version is set ONLY IN THE MAKEFILE!  Don't delete this!
VERSION=3.11

DEFINES=
prefix=/usr
exec_prefix=${prefix}
mandir=${prefix}/share/man
docdir=/usr/share/doc/rp-pppoe-$(VERSION)
install=/usr/bin/install -c
install_dir=/usr/bin/install -c -d
sbindir=/sbin

# Plugin for pppd on Linux
LINUX_KERNELMODE_PLUGIN=rp-pppoe.so
PPPD_INCDIR=/home/wyj/at960/ip-phone/rainbow/rainbow4/ifx-com-linux/source/user/opensource/open_newpppoe/

# Licensed version
LIC_INCDIR=
LIC_LIBDIR=
LIC_LIB=
LIC_DEFINE=

# PPPoE relay -- currently only supported on Linux
PPPOE_RELAY=pppoe-relay

# Program paths
PPPOE_PATH=$(sbindir)/pppoe
PPPD_PATH=/home/wyj/at960/ip-phone/rainbow/rainbow4/ifx-com-linux/source/user/opensource/open_newpppoe/pppd/pppd

# Include ServPoET version if we're building for ServPoET


# Kernel-mode plugin gets installed here.
PLUGIN_DIR=/etc/ppp/plugins
PLUGIN_PATH=$(PLUGIN_DIR)/rp-pppoe.so

# Configuration file paths
PPPOESERVER_PPPD_OPTIONS=/etc/ppp/pppoe-server-options

PATHS='-DPPPOE_PATH="$(PPPOE_PATH)"' '-DPPPD_PATH="$(PPPD_PATH)"' \
	'-DPLUGIN_PATH="$(PLUGIN_PATH)"' \
	'-DPPPOE_SERVER_OPTIONS="$(PPPOESERVER_PPPD_OPTIONS)"'

CFLAGS= -g -O2 -fno-strict-aliasing -Wall -Wstrict-prototypes $(LIC_INCDIR) $(DEFINES) $(LIC_DEFINE) $(PATHS) -Ilibevent
TARGETS=pppoe pppoe-server pppoe-sniff pppoe-relay
PPPOE_SERVER_LIBS=$(LIC_LIBDIR) $(LIC_LIB)
LDFLAGS= 

all: $(TARGETS)
	@echo ""
	@echo "Type 'make install' as root to install the software."

pppoe-sniff: pppoe-sniff.o if.o common.o debug.o
	$(CC) -o $@ $^ $(LDFLAGS)

pppoe-server: pppoe-server.o if.o debug.o common.o md5.o libevent/libevent.a 
	$(CC) -o $@  $^ $(LDFLAGS) $(PPPOE_SERVER_LIBS) -Llibevent -levent

pppoe: pppoe.o if.o debug.o common.o ppp.o discovery.o
	$(CC) -o $@ $^ $(LDFLAGS)

pppoe-relay: relay.o if.o debug.o common.o
	$(CC) -o $@ $^ $(LDFLAGS)

pppoe.o: pppoe.c pppoe.h
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ $<

discovery.o: discovery.c pppoe.h
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ $<

ppp.o: ppp.c pppoe.h
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ $<

md5.o: md5.c md5.h
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ $<

pppoe-server.o: pppoe-server.c pppoe.h 
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ $<

pppoe-sniff.o: pppoe-sniff.c pppoe.h
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ $<

if.o: if.c pppoe.h
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ $<

libevent/libevent.a:
	cd libevent && $(MAKE) DEFINES="$(DEFINES)"

common.o: common.c pppoe.h
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ $<

debug.o: debug.c pppoe.h
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ $<

relay.o: relay.c relay.h pppoe.h
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ $<

# Linux-specific plugin
rp-pppoe.so: plugin/plugin.o plugin/libplugin.a
	$(CC) -o $@ -shared $^ $(LDFLAGS)

plugin/plugin.o: plugin.c
	$(CC) -DPLUGIN=1 '-DRP_VERSION="$(VERSION)"' $(CFLAGS) -I$(PPPD_INCDIR) -c -o $@ -fPIC $<

plugin/libplugin.a: plugin/discovery.o plugin/if.o plugin/common.o plugin/debug.o
	$(AR) -rc $@ $^

plugin/discovery.o: discovery.c
	$(CC) -DPLUGIN=1 $(CFLAGS) -I$(PPPD_INCDIR) '-DVERSION="$(VERSION)"' -c -o $@ -fPIC $<

plugin/if.o: if.c
	$(CC) -DPLUGIN=1 $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ -fPIC $<

plugin/debug.o: debug.c
	$(CC) -DPLUGIN=1 $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ -fPIC $<

plugin/common.o: common.c
	$(CC) -DPLUGIN=1 $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o $@ -fPIC $<

install: all
	-mkdir -p $(DESTDIR)$(sbindir)
	$(install) -m 755 pppoe $(DESTDIR)$(sbindir)/
	-mkdir -p $(DESTDIR)/etc/ppp
	-mkdir -p $(DESTDIR)$(PLUGIN_DIR)
	-echo "# Directory created by rp-pppoe for kernel-mode plugin" > $(DESTDIR)$(PLUGIN_DIR)/README
	@if test -r rp-pppoe.so; then $(install) -m 755 rp-pppoe.so $(DESTDIR)$(PLUGIN_DIR); fi
	@echo "Type 'pppoe-setup' to configure the software."

servpoet-tgz: distro-servpoet
	cd .. && tar cvf servpoet-$(VERSION)$(BETA).tar servpoet-$(VERSION)$(BETA)
	gzip -f -v -9 ../servpoet-$(VERSION)$(BETA).tar
	rm -rf ../servpoet-$(VERSION)$(BETA)

tgz: distro
	cd .. && tar cvf rp-pppoe-$(VERSION)$(BETA).tar rp-pppoe-$(VERSION)$(BETA)
	gzip -f -v -9 ../rp-pppoe-$(VERSION)$(BETA).tar
	rm -rf ../rp-pppoe-$(VERSION)$(BETA)

distro-servpoet: distro
	cp ../servpoet.spec ../rp-pppoe-$(VERSION)$(BETA)
	$(MAKE) -C licensed-only distro VERSION=$(VERSION) BETA=$(BETA)
	mv ../rp-pppoe-$(VERSION)$(BETA) ../servpoet-$(VERSION)$(BETA)

distro:
	rm -rf ../rp-pppoe-$(VERSION)$(BETA)
	mkdir ../rp-pppoe-$(VERSION)$(BETA)
	for i in README SERVPOET go go-gui rp-pppoe.spec ; do \
		cp ../$$i ../rp-pppoe-$(VERSION)$(BETA) || exit 1; \
	done
	mkdir ../rp-pppoe-$(VERSION)$(BETA)/gui
	for i in en.msg ja.msg Makefile.in tkpppoe.in wrapper.c tkpppoe.1 pppoe-wrapper.1 ; do \
		cp ../gui/$$i ../rp-pppoe-$(VERSION)$(BETA)/gui || exit 1; \
	done
	mkdir ../rp-pppoe-$(VERSION)$(BETA)/gui/html
	for i in mainwin-busy.png mainwin-nonroot.png mainwin.png props-advanced.png props-basic.png props-nic.png props-options.png tkpppoe.html ; do \
		cp ../gui/html/$$i ../rp-pppoe-$(VERSION)$(BETA)/gui/html || exit 1; \
	done
	mkdir ../rp-pppoe-$(VERSION)$(BETA)/configs
	for i in firewall-masq firewall-standalone pap-secrets pppoe-server-options pppoe.conf ; do \
		cp ../configs/$$i ../rp-pppoe-$(VERSION)$(BETA)/configs || exit 1; \
	done
	mkdir ../rp-pppoe-$(VERSION)$(BETA)/doc
	for i in CHANGES KERNEL-MODE-PPPOE HOW-TO-CONNECT LICENSE PROBLEMS ; do \
		cp ../doc/$$i ../rp-pppoe-$(VERSION)$(BETA)/doc || exit 1; \
	done
	mkdir ../rp-pppoe-$(VERSION)$(BETA)/man
	for i in pppoe-connect.8 pppoe-setup.8 pppoe-start.8 pppoe-status.8 pppoe-stop.8 pppoe-server.8 pppoe-sniff.8 pppoe.8 pppoe-relay.8 pppoe.conf.5 ; do \
		cp ../man/$$i ../rp-pppoe-$(VERSION)$(BETA)/man || exit 1; \
	done
	mkdir ../rp-pppoe-$(VERSION)$(BETA)/scripts
	for i in pppoe-connect.in pppoe-init-suse.in pppoe-init-turbolinux.in pppoe-init.in pppoe-setup.in pppoe-start.in pppoe-stop.in pppoe-status ; do \
		cp ../scripts/$$i ../rp-pppoe-$(VERSION)$(BETA)/scripts || exit 1; \
	done
	mkdir ../rp-pppoe-$(VERSION)$(BETA)/src
	for i in Makefile.in install-sh common.c config.h.in configure configure.in debug.c discovery.c if.c md5.c md5.h ppp.c pppoe-server.c pppoe-sniff.c pppoe.c pppoe.h pppoe-server.h plugin.c relay.c relay.h ; do \
		cp ../src/$$i ../rp-pppoe-$(VERSION)$(BETA)/src || exit 1; \
	done
	mkdir ../rp-pppoe-$(VERSION)$(BETA)/src/libevent
	for i in Makefile.in event.c event.h event_tcp.c event_tcp.h eventpriv.h hash.c hash.h event_sig.c ; do \
		cp ../src/libevent/$$i ../rp-pppoe-$(VERSION)$(BETA)/src/libevent || exit 1; \
	done
	mkdir ../rp-pppoe-$(VERSION)$(BETA)/src/plugin

distro-beta: beta-check
	$(MAKE) distro BETA=-BETA-$(BETA)

tgz-beta: beta-check
	$(MAKE) tgz BETA=-BETA-$(BETA)

beta-check:
	@if test "$(BETA)" = "" ; then \
		echo "Usage: make distro-beta BETA=<x>"; \
		exit 1; \
	fi

rpms: tgz
	cp ../rp-pppoe-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	gpg --detach-sign /usr/src/redhat/SOURCES/rp-pppoe-$(VERSION).tar.gz
	cd ..; \
	rpm -ba rp-pppoe.spec
	rpm --addsign /usr/src/redhat/SRPMS/rp-pppoe-$(VERSION)-1.src.rpm \
		/usr/src/redhat/RPMS/i386/rp-pppoe-$(VERSION)-1.i386.rpm \
		/usr/src/redhat/RPMS/i386/rp-pppoe-gui-$(VERSION)-1.i386.rpm

servpoet-rpms: servpoet-tgz
	cp ../servpoet-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	cd .. && rpm -ba servpoet.spec

clean:
	rm -f *.o pppoe-relay pppoe pppoe-sniff pppoe-server core rp-pppoe.so plugin/*.o plugin/libplugin.a *~
	test -f licensed-only/Makefile && $(MAKE) -C licensed-only clean || true
	test -f libevent/Makefile && $(MAKE) -C libevent clean || true
	test -f l2tp/Makefile && $(MAKE) -C l2tp clean || true

distclean: clean
	rm -f Makefile config.h config.cache config.log config.status
	rm -f libevent/Makefile
	rm -f ../scripts/pppoe-connect ../scripts/pppoe-start ../scripts/pppoe-stop ../scripts/pppoe-init ../scripts/pppoe-setup ../scripts/pppoe-init-suse ../scripts/pppoe-init-turbolinux

update-version:
	sed -e 's/^Version: .*$$/Version: $(VERSION)/' ../rp-pppoe.spec > ../rp-pppoe.spec.new && mv ../rp-pppoe.spec.new ../rp-pppoe.spec
	sed -e 's+^Source: .*$$+Source: http://www.roaringpenguin.com/pppoe/rp-pppoe-$(VERSION).tar.gz+' ../rp-pppoe.spec > ../rp-pppoe.spec.new && mv ../rp-pppoe.spec.new ../rp-pppoe.spec
	test -f ../servpoet.spec && sed -e 's/^Version: .*$$/Version: $(VERSION)/' ../servpoet.spec > ../servpoet.spec.new && mv ../servpoet.spec.new ../servpoet.spec || true
	test -f ../servpoet.spec && sed -e 's+^Source: .*$$+Source: http://www.roaringpenguin.com/pppoe/servpoet-$(VERSION).tar.gz+' ../servpoet.spec > ../servpoet.spec.new && mv ../servpoet.spec.new ../servpoet.spec || true

# Convenience target for David!  Don't try to use this one.
km:
	./configure --enable-plugin=/home/dfs/Archive/PPP/ppp-2.4.0.pppoe4-patched-dfs

licensed-only:
	$(MAKE) -C licensed-only all VERSION=$(VERSION) SERVPOET_VERSION=$(SERVPOET_VERSION)

l2tp: libevent/libevent.a
	$(MAKE) -C l2tp all

.PHONY: update-version

.PHONY: clean

.PHONY: distclean

.PHONY: rpms

.PHONY: licensed-only

.PHONY: distro

.PHONY: l2tp
