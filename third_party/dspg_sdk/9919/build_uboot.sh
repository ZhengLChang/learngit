#!/bin/sh

if [[ $EUID -ne 0 ]]; then
    echo "please use sudo"
    exit 1
fi

source /opt/dspg/v1.3.0-rc4/environment-setup-armv5te-dspg-linux-gnueabi
export CROSS_COMPILE=arm-dspg-linux-gnueabi-
export CROSS=arm-dspg-linux-gnueabi-
export AR=arm-dspg-linux-gnueabi-ar
export RANLIB=arm-dspg-linux-gnueabi-ranlib
export STRIP=arm-dspg-linux-gnueabi-strip


CPU_COUNT=`cat /proc/cpuinfo | grep processor | wc -l`
cd ./u-boot
git clean -x -d -f 
make distclean
make evb994lrev2_nand_config
make -j$CPU_COUNT
