#!/bin/bash
APPS_NAME="dhcpcd-new"
ls -l
if [ -e sysconfig.sh ]; then
        . sysconfig.sh
        . config.sh
        . model_config.sh
else
        echo "Application "$APPS_NAME" not configured"
        exit 1;
fi

echo "----------------------------------------------------------------------"
echo "-----------------------      build dhcpcd-new  ------------------------"
echo "----------------------------------------------------------------------"

parse_args $@

if [ "$1" = "config_only" -a ! -f .config_ok -o $BUILD_CONFIGURE -eq 1 ]; then
        aclocal
        autoconf
        automake --add-missing
        . ./configure --host=mips-linux
        ifx_error_check $?
        echo -n > .config_ok
fi

if [ "$1" = "config_only" ] ;then
        exit 0
fi

if [ $BUILD_CLEAN -eq 1 ]; then
        rm -rf .config_ok
        make -C src clean
        [ ! $BUILD_CONFIGURE -eq 1 ] && exit 0
fi

make AR="$IFX_AR" AS="$IFX_AS" LD="$IFX_LD" NM="$IFX_NM" CC="$IFX_CC" BUILDCC="$IFX_HOSTCC" GCC="$IFX_CC" CXX="$IFX_CXX" CPP="$IFX_CPP" RANLIB="$IFX_RANLIB" IFX_CFLAGS="$IFX_CFLAGS" IFX_LDFLAGS="$IFX_LDFLAGS"
ifx_error_check $?

${IFX_STRIP} dhcpcd

install -d "$BUILD_ROOTFS_DIR"/usr/sbin
cp -f dhcpcd "$BUILD_ROOTFS_DIR"/usr/sbin
ifx_error_check $?
