#!/bin/sh

if [[ $EUID -ne 0 ]]; then
	echo "please use sudo"
	exit 1
fi

set -e
source /opt/dspg/v1.3.0-rc4/environment-setup-armv5te-dspg-linux-gnueabi

$CC -o uboot_upgrade_mkfile uboot_upgrade_mkfile.c -Wall

rm rootfs -rf
tar xvf rootfs_9919.tar.gz

cp -f uImage rootfs/boot/
cp -f dvf99-evb.dtb rootfs/boot/
cp -f *.ko rootfs/usr/local/drivers/
cp -f uboot_upgrade_mkfile rootfs/usr/sbin/

mkfs.ubifs -m 2048 -e 126976 -c 2047 -r rootfs -o rootfs.img
#ubinize -o rootfs.img -m 2048 -p 131072 ubinize.cfg
rm rootfs -rf
