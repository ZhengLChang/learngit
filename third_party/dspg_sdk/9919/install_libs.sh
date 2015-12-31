#!/bin/sh

if [[ $EUID -ne 0 ]]; then
    echo "please use sudo"
    exit 1
fi

source /opt/dspg/v1.3.0-rc4/environment-setup-armv5te-dspg-linux-gnueabi

rm -rf applibs
mkdir applibs
mkdir applibs/include
mkdir applibs/libs
cp tools/libcoma/inc/coma_service_api.h applibs/include
cp tools/libcoma/linux/libcoma.a applibs/libs
cp tools/libdua/inc/*.h applibs/include
cp tools/libdua/release/dua/* applibs/include
cp tools/libdua/linux/libdua.a applibs/libs
cp tools/include/*.h applibs/include
cp tools/voip-app/librtp/rtp.h applibs/include
cp tools/voip-app/librtp/librtp.a applibs/libs
cp tools/voip-app/include/voice.h applibs/include
cp tools/libmem/libmem.a applibs/libs
cp tools/libdaif/*.h applibs/include

if [ $OECORE_TARGET_SYSROOT ] 
then
	rm -rf $OECORE_TARGET_SYSROOT/applibs
	cp -R applibs $OECORE_TARGET_SYSROOT/
else
	echo "env error!!"
fi

echo "************************************"
echo "=============cp /usr/bin/dua & /usr/bin/dwt to rootfs"
echo "************************************"
