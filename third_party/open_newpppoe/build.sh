#!/bin/bash
APPS_NAME="ppp-2.4.4"
if [ -e sysconfig.sh ]; then
	. sysconfig.sh
else
	echo "Application "$APPS_NAME" not configured"
fi

echo "----------------------------------------------------------------------"
echo "-----------------------      build ppp-2.4.4  ------------------------"
echo "----------------------------------------------------------------------"

parse_args $@

if [ $BUILD_CLEAN -eq 1 ]; then
	rm -rf .config_ok
	make clean
	[ ! $BUILD_CONFIGURE -eq 1 ] && exit 0
fi

if [ "$1" = "config_only" -a ! -f .config_ok -o $BUILD_CONFIGURE -eq 1 ]; then
	echo -n > .config_ok
fi

if [ "$1" = "config_only" ] ;then
	exit 0
fi

#if [ $BUILD_2MB_PACKAGE -eq 1 ];then
#	IFX_CFLAGS="${IFX_CFLAGS} -DIFX_SMALL_FOOTPRINT"
#	make AR=${IFX_AR} AS=${IFX_AS} LD=${IFX_LD} NM=${IFX_NM} CC=${IFX_CC} BUILDCC=${IFX_HOSTCC} GCC=${IFX_CC} CXX=${IFX_CXX} CPP=${IFX_CPP} RANLIB=${IFX_RANLIB} IFX_CFLAGS="${IFX_CFLAGS}" IFX_LDFLAGS="${IFX_LDFLAGS}" IFX_2MB_PPP_PKG=1 install DESTDIR=${BUILD_ROOTFS_DIR}
#else
	make AR=${IFX_AR} STRIP=${IFX_STRIP} AS=${IFX_AS} LD=${IFX_LD} NM=${IFX_NM} CC=${IFX_CC} BUILDCC=${IFX_HOSTCC} GCC=${IFX_CC} CXX=${IFX_CXX} CPP=${IFX_CPP} RANLIB=${IFX_RANLIB} IFX_CFLAGS="${IFX_CFLAGS}" IFX_LDFLAGS="${IFX_LDFLAGS}" install DESTDIR=${BUILD_ROOTFS_DIR}
#fi
ifx_error_check $? 

