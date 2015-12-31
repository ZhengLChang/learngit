#!/bin/bash
source /opt/dspg/v1.3.1.1-rc1/environment-setup-armv5te-dspg-linux-gnueabi
export CROSS_COMPILE=arm-dspg-linux-gnueabi-
export CROSS=arm-dspg-linux-gnueabi-
export AR=arm-dspg-linux-gnueabi-ar
export RANLIB=arm-dspg-linux-gnueabi-ranlib
export STRIP=arm-dspg-linux-gnueabi-strip

#get our pc's pthread numbers to accacurate compile time 
CPU_COUNT=`cat /proc/cpuinfo | grep processor | wc -l`
PWD_PATH=`pwd`
cd switch
make 
cd ${PWD_PATH}

make 
make install INSTALL_PATH=/home/yml/work/DSPG/atcom_9919/rootfs/usr/driver/modules

