##################################################
# Copyright (C), 2010-2013, ATCOM Tech. Co., Ltd.
# Filename: Makefile
# Author: hyh <hyf@atcom.com.cn>
# Created: Wed Oct  9 15:56:00 CST 2013
##################################################

LIBMXML := $(CURDIR)/libmxml-0.9.1/lib/libmxml.a
BINDHCPC := $(CURDIR)/open_dhcpclient/output/sbin/dhcpcd
BINL2TPD := $(CURDIR)/open_l2tpd/l2tpd
BINXL2TPD := $(CURDIR)/open_xl2tpd/xl2tpd
BINNTPC := $(CURDIR)/open_ntpclient/ntpclient
BINPPPOE := $(CURDIR)/open_newpppoe/pppd/pppd
BINRPSERV := $(CURDIR)/open_rpserver/src/pppoe
BINPPPD := $(CURDIR)/open_ppp/pppd/pppd
BINTCPDUMP := $(CURDIR)/open_tcpdump/tcpdump
#BINIPTABLES := $(CURDIR)iptables-1.4.12.2/output/sbin/xtables-multi     #use toolchain iptables
LIBWPA := $(CURDIR)/wpa_supplicant_hostapd/wpa_supplicant/wpa_supplicant
BINWGET := $(CURDIR)/wget-1.12/src/wget
LIBLDAP := $(CURDIR)/openldap-2.4.40/output/lib/libldap.a
LIBPCAP := $(CURDIR)/open_tcpdump/libpcap/libpcap.a
LIBUARTPROTO := $(CURDIR)/uart_protocol/libprotocol.a

contrib_libs_9918 := $(LIBMXML) $(BINDHCPC) $(BINL2TPD) $(BINXL2TPD) $(BINNTPC) $(BINPPPOE) $(BINPPPD) $(LIBPCAP) $(BINTCPDUMP) $(LIBWPA) $(LIBLDAP) $(BINWGET) $(BINRPSERV) 
contrib_libs_9919 := $(LIBMXML) $(BINDHCPC) $(BINL2TPD) $(BINXL2TPD) $(BINNTPC) $(BINPPPOE) $(BINPPPD) $(LIBPCAP) $(BINTCPDUMP) $(LIBWPA) $(LIBLDAP) $(BINWGET) $(BINRPSERV) $(LIBUARTPROTO)
contrib_libs_pc := $(LIBMXML) $(LIBUARTPROTO)

CC := $(TARGET_PREFIX)gcc
LD := $(TARGET_PREFIX)ld
AR := $(TARGET_PREFIX)ar
RANLIB := $(TARGET_PREFIX)ranlib
STRIP := $(TARGET_PREFIX)strip

CFLAGS = -Wall -g
export CFLAGS
export CC
export LD
export AR
export RANLIB
export STRIP

all :
	@echo "======usage:"
	@echo "make clean && make 9918 && make install_9918"
	@echo "make clean && make 9919 && make install_9919"
	@echo "make clean && make pc_test && make install_pc"
	@echo

9918: check_env_9918 $(contrib_libs_9918)

9919: check_env_9919 $(contrib_libs_9919)

pc_test: check_env_pc $(contrib_libs_pc)

PHONY += check_env_9918
check_env_9918:
	@if [ ! $(CC) == arm-dspg-linux-uclibceabi-gcc ];then echo "==please set 9918 evn"; exit -1 ;fi
	@if [[ $(PATH) =~ "armv5te-dspg-linux-gnueabi" ]];then echo "==please set 9918 evn"; exit -1 ;fi

PHONY += check_env_9919
check_env_9919:
	@if [ ! $(CC) == arm-dspg-linux-gnueabi-gcc ];then echo "==please set 9919 env"; exit -1 ;fi
	@if [[ $(PATH) =~ "armv5te-dspg-linux-uclibceabi" ]];then echo "==please set 9919 evn"; exit -1 ;fi

PHONY += check_env_pc
check_env_pc:
	@if [ ! $(CC) == gcc ];then echo "==please clear env"; exit -1 ;fi

$(CURDIR)/open_tcpdump/libpcap/Makefile:
	@cd $(@D) && chmod +x ./configure && ./configure $(CONFIGURE_FLAGS) --with-pcap=linux

$(LIBPCAP): $(CURDIR)/open_tcpdump/libpcap/Makefile
	@cd $(^D) && $(MAKE)


$(CURDIR)/libmxml-0.9.1/Makefile:
	@cd $(@D) && aclocal && autoconf && automake --add-missing && chmod +x ./configure && ./configure $(CONFIGURE_FLAGS)

$(LIBMXML): $(CURDIR)/libmxml-0.9.1/Makefile
	@cd $(^D) && $(MAKE)

$(CURDIR)/open_dhcpclient/Makefile:
	@cd $(@D) && aclocal && autoconf && automake --add-missing && ./configure --prefix=$(CURDIR)/open_dhcpclient/output $(CONFIGURE_FLAGS)

$(BINDHCPC): $(CURDIR)/open_dhcpclient/Makefile
	@cd $(^D) && $(MAKE) && $(MAKE) install

$(BINL2TPD):
	@cd $(@D) && $(MAKE)
	
$(BINXL2TPD):
	@cd $(@D) && $(MAKE)

$(BINNTPC):
	@cd $(@D) && $(MAKE)


$(BINPPPOE):
	@cd $(@D) && $(MAKE) install DESTDIR=$(CURDIR)/open_newpppoe/output

$(BINRPSERV):
	@cd $(@D) && $(MAKE)  install DESTDIR=$(CURDIR)/open_rpserver/output 

$(CURDIR)/open_ppp/Makefile:
	@cd $(@D) && ./configure $(CONFIGURE_FLAGS)

$(BINPPPD): $(CURDIR)/open_ppp/Makefile
	@cd $(^D) && $(MAKE) install DESTDIR=$(CURDIR)/open_ppp/output

$(CURDIR)/open_tcpdump/Makefile:
	@cd $(@D) && ./configure --prefix=$(CURDIR)/open_tcpdump/output $(CONFIGURE_FLAGS)

$(BINTCPDUMP): $(CURDIR)/open_tcpdump/Makefile
	@cd $(^D) && $(MAKE) && $(MAKE) install

$(CURDIR)/iptables-1.4.12.2/Makefile:
	@cd $(@D) && ./configure --disable-ipv6 --prefix=$(CURDIR)/iptables-1.4.12.2/output $(CONFIGURE_FLAGS)

$(BINIPTABLES): $(CURDIR)/iptables-1.4.12.2/Makefile
	@cd $(^D) && $(MAKE) && $(MAKE) install

$(LIBWPA):
	@cd $(@D) && $(MAKE)


$(CURDIR)/openldap-2.4.40/Makefile:
	@cd $(@D) && chmod +x ./configure && ./configure --prefix=$(CURDIR)/openldap-2.4.40/output $(CONFIGURE_FLAGS) --enable-slapd=no --enable-bdb=no --enable-hdb=no  --enable-mdb=no --with-yielding_select=yes --with-tls=no

$(LIBLDAP): $(CURDIR)/openldap-2.4.40/Makefile
	@cd $(^D) && $(MAKE) depend && $(MAKE) && $(MAKE) install

$(CURDIR)/wget-1.12/Makefile:
	@cd $(@D) && ./configure $(CONFIGURE_FLAGS) --enable-shared=yes --disable-ipv6 --without-zlib --disable-iri --disable-nls --disable-ntlm --with-ssl=openssl --with-openssl=auto

$(BINWGET): $(CURDIR)/wget-1.12/Makefile
	@cd $(^D) && $(MAKE)
	

$(LIBUARTPROTO):
	@cd $(@D) && $(MAKE)

PHONY += clean
clean:
	- @cd $(CURDIR)/open_dhcpclient && $(MAKE) uninstall && $(MAKE) clean && $(MAKE) distclean
	- @cd $(CURDIR)/open_l2tpd && $(MAKE) clean
	- @cd $(CURDIR)/open_xl2tpd && $(MAKE) clean
	- @cd $(CURDIR)/open_ntpclient && $(MAKE) clean
	- @cd $(CURDIR)/open_newpppoe && $(MAKE) clean
	- @rm $(CURDIR)/open_newpppoe/pppd/pppd
	- @cd $(CURDIR)/open_rpserver/src && $(MAKE) clean
	- @rm $(CURDIR)/open_rpserver/src/pppoe
	- @cd $(CURDIR)/open_ppp && $(MAKE) clean
	- @cd $(CURDIR)/open_tcpdump && $(MAKE) uninstall && $(MAKE) clean && $(MAKE) distclean
	- @cd $(CURDIR)/open_tcpdump/libpcap && $(MAKE) clean && $(MAKE) distclean
	- @rm -rf $(CURDIR)/open_tcpdump/output/*
	- @cd $(CURDIR)/wpa_supplicant_hostapd/wpa_supplicant && $(MAKE) clean
	- @cd $(CURDIR)/openldap-2.4.40 && $(MAKE) clean && $(MAKE) distclean
	- @cd $(CURDIR)/wget-1.12 && $(MAKE) uninstall && $(MAKE) clean && $(MAKE) distclean
	- @cd $(CURDIR)/libmxml-0.9.1 && $(MAKE) clean && $(MAKE) distclean
	- @cd $(CURDIR)/uart_protocol && $(MAKE) clean


PHONY += clean_9918
clean_9918:
	- @rm -rf $(CURDIR)/output/include/*
	- @rm -rf $(CURDIR)/output/bin/*
	- @rm -rf $(CURDIR)/output/lib/*

PHONY += clean_9919
clean_9919:
	- @rm -rf $(CURDIR)/output_9919/include/*
	- @rm -rf $(CURDIR)/output_9919/bin/*
	- @rm -rf $(CURDIR)/output_9919/lib/*

PHONY += clean_pc
clean_pc:
	- @rm -rf $(CURDIR)/output_pc/include/*
	- @rm -rf $(CURDIR)/output_pc/bin/*
	- @rm -rf $(CURDIR)/output_pc/lib/*



PHONY += install_9918
install_9918: clean_9918
	cp -i $(CURDIR)/open_dhcpclient/output/sbin/dhcpcd $(CURDIR)/output/bin/
	$(STRIP) -s $(CURDIR)/output/bin/dhcpcd
	cp -i $(CURDIR)/open_l2tpd/l2tpd $(CURDIR)/output/bin/
	$(STRIP) -s $(CURDIR)/output/bin/l2tpd
	cp -i $(CURDIR)/open_ntpclient/ntpclient $(CURDIR)/output/bin/
	$(STRIP) -s $(CURDIR)/output/bin/ntpclient
	cp -i $(CURDIR)/open_ppp/output/usr/sbin/pppd $(CURDIR)/output/bin/
	chmod +w $(CURDIR)/output/bin/pppd
	$(STRIP) -s $(CURDIR)/output/bin/pppd
	cp -i $(CURDIR)/open_tcpdump/output/sbin/tcpdump $(CURDIR)/output/bin/
	$(STRIP) -s $(CURDIR)/output/bin/tcpdump
	cp -i $(CURDIR)/open_tcpdump/libpcap/libpcap.a $(CURDIR)/output/lib/
	cp -ri $(CURDIR)/open_tcpdump/libpcap/pcap $(CURDIR)/output/include/
	cp -i $(CURDIR)/wpa_supplicant_hostapd/wpa_supplicant/wpa_supplicant $(CURDIR)/output/bin/
	$(STRIP) -s $(CURDIR)/output/bin/wpa_supplicant
	cp -i $(CURDIR)/wpa_supplicant_hostapd/wpa_supplicant/libwpa_cli.a $(CURDIR)/output/lib/
	cp -i $(CURDIR)/wpa_supplicant_hostapd/wpa_supplicant/at_wpa_cli.h $(CURDIR)/output/include/
	cp -i $(CURDIR)/openldap-2.4.40/libraries/liblutil/liblutil.a $(CURDIR)/output/lib/
	cp -i $(CURDIR)/openldap-2.4.40/output/lib/liblber.a $(CURDIR)/output/lib/
	cp -i $(CURDIR)/openldap-2.4.40/output/lib/libldap.a $(CURDIR)/output/lib/
	cp -i $(CURDIR)/wget-1.12/src/wget $(CURDIR)/output/bin/
	$(STRIP) -s $(CURDIR)/output/bin/wget
	cp -i $(CURDIR)/libmxml-0.9.1/lib/libmxml.a $(CURDIR)/output/lib/
	cp -i $(CURDIR)/libmxml-0.9.1/include/*.h $(CURDIR)/output/include/


PHONY += install_9919
install_9919: clean_9919
	cp -i $(CURDIR)/open_dhcpclient/output/sbin/dhcpcd $(CURDIR)/output_9919/bin/
	$(STRIP) -s $(CURDIR)/output_9919/bin/dhcpcd
	cp -i $(CURDIR)/open_l2tpd/l2tpd $(CURDIR)/output_9919/bin/
	$(STRIP) -s $(CURDIR)/output_9919/bin/l2tpd
	cp -i $(CURDIR)/open_xl2tpd/xl2tpd $(CURDIR)/output_9919/bin/
	$(STRIP) -s $(CURDIR)/output_9919/bin/xl2tpd
	cp -i $(CURDIR)/open_ntpclient/ntpclient $(CURDIR)/output_9919/bin/
	$(STRIP) -s $(CURDIR)/output_9919/bin/ntpclient
	cp -i $(CURDIR)/open_newpppoe/output/sbin/pppd2 $(CURDIR)/output_9919/bin/
	$(STRIP) -s $(CURDIR)/output_9919/bin/pppd2
	cp -i $(CURDIR)/open_rpserver/output/sbin/pppoe $(CURDIR)/output_9919/bin/
	$(STRIP) -s $(CURDIR)/output_9919/bin/pppoe
	cp -i $(CURDIR)/open_ppp/output/usr/sbin/pppd $(CURDIR)/output_9919/bin/
	chmod +w $(CURDIR)/output_9919/bin/pppd
	$(STRIP) -s $(CURDIR)/output_9919/bin/pppd
	cp -i $(CURDIR)/open_tcpdump/output/sbin/tcpdump $(CURDIR)/output_9919/bin/
	$(STRIP) -s $(CURDIR)/output_9919/bin/tcpdump
	cp -i $(CURDIR)/open_tcpdump/libpcap/libpcap.a $(CURDIR)/output_9919/lib/
	cp -ri $(CURDIR)/open_tcpdump/libpcap/pcap $(CURDIR)/output_9919/include/
	cp -i $(CURDIR)/wpa_supplicant_hostapd/wpa_supplicant/wpa_supplicant $(CURDIR)/output_9919/bin/
	$(STRIP) -s $(CURDIR)/output_9919/bin/wpa_supplicant
	cp -i $(CURDIR)/wpa_supplicant_hostapd/wpa_supplicant/libwpa_cli.a $(CURDIR)/output_9919/lib/
	cp -i $(CURDIR)/wpa_supplicant_hostapd/wpa_supplicant/at_wpa_cli.h $(CURDIR)/output_9919/include/
	cp -i $(CURDIR)/openldap-2.4.40/libraries/liblutil/liblutil.a $(CURDIR)/output_9919/lib/
	cp -i $(CURDIR)/openldap-2.4.40/output/lib/liblber.a $(CURDIR)/output_9919/lib/
	cp -i $(CURDIR)/openldap-2.4.40/output/lib/libldap.a $(CURDIR)/output_9919/lib/
	cp -i $(CURDIR)/wget-1.12/src/wget $(CURDIR)/output_9919/bin/
	$(STRIP) -s $(CURDIR)/output_9919/bin/wget
	cp -i $(CURDIR)/libmxml-0.9.1/lib/libmxml.a $(CURDIR)/output_9919/lib/
	cp -i $(CURDIR)/libmxml-0.9.1/include/*.h $(CURDIR)/output_9919/include/
	cp -i $(CURDIR)/uart_protocol/libprotocol.a $(CURDIR)/output_9919/lib/
	cp -i $(CURDIR)/uart_protocol/app/app_proto.h $(CURDIR)/output_9919/include/
	cp -i $(CURDIR)/uart_protocol/link/link_proto.h $(CURDIR)/output_9919/include/
	cp -i $(CURDIR)/uart_protocol/tty/tty_proto.h $(CURDIR)/output_9919/include/


PHONY += install_pc
install_pc: clean_pc
	cp -i $(CURDIR)/libmxml-0.9.1/lib/libmxml.a $(CURDIR)/output_pc/lib/
	cp -i $(CURDIR)/libmxml-0.9.1/include/*.h $(CURDIR)/output_pc/include/
	cp -i $(CURDIR)/uart_protocol/libprotocol.a $(CURDIR)/output_pc/lib/
	cp -i $(CURDIR)/uart_protocol/app/app_proto.h $(CURDIR)/output_pc/include/
	cp -i $(CURDIR)/uart_protocol/link/link_proto.h $(CURDIR)/output_pc/include/
	cp -i $(CURDIR)/uart_protocol/tty/tty_proto.h $(CURDIR)/output_pc/include/


PHONY += help
help:
	@echo ======use "make clean" to clean all project
	@echo ======use "make clean_9918" to clean 9918 .h .bin .so .a
	@echo ======use "make clean_9919" to clean 9919 .h .bin .so .a
	@echo 
	@echo 
