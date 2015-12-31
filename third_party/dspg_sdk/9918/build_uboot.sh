#!/bin/sh

if [[ $EUID -ne 0 ]]; then
    echo "please use sudo"
    exit 1
fi

source /opt/dspg/v1.3.0-rc4/environment-setup-armv5te-dspg-linux-uclibceabi
export CROSS_COMPILE=arm-dspg-linux-uclibceabi-
export CROSS=arm-dspg-linux-uclibceabi-
export AR=arm-dspg-linux-uclibceabi-ar
export RANLIB=arm-dspg-linux-uclibceabi-ranlib
export STRIP=arm-dspg-linux-uclibceabi-strip


CPU_COUNT=`cat /proc/cpuinfo | grep processor | wc -l`
cd ./u-boot
git clean -x -d -f 
make distclean
make evb9918_nand_config
make -j$CPU_COUNT
