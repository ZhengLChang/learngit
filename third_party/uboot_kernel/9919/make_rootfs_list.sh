#!/bin/sh
if [[ $EUID -ne 0 ]]; then
    echo "please use sudo"
	exit 1
fi

mkdir tmp_mklist
cd tmp_mklist/
mkdir boot
mkdir usr
mkdir usr/local
mkdir usr/local/drivers
cp ../uImage boot/
cp ../dvf99-evb.dtb boot/
cp ../*.ko usr/local/drivers/
tar zcvf ../rootfs_list.tar.gz boot usr
cd ..
rm -rf tmp_mklist
