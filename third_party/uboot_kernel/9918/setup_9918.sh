#!/bin/sh

if [[ $EUID -ne 0 ]]; then
	echo "please use sudo"
	exit 1
fi

set -e
source /opt/dspg/v1.3.0-rc4/environment-setup-armv5te-dspg-linux-uclibceabi

rm rootfs -rf
rm app -rf
tar xvf rootfs_9918.tar.gz
tar xvf app.tar.gz

cp uImage rootfs/boot/
cp dvf9918-evb.dtb rootfs/boot/
cp -R app rootfs/usr/local/

mkfs.jffs2 --little-endian --pad --eraseblock=65536 -r rootfs -o rootfs.jffs2
rm rootfs -rf
rm app -rf
