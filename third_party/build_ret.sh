#!/bin/sh

export PATH="$PATH:/home/atcom-projects/ip-phone/toolchain/ifx-lxdb-1-2/gcc-3.3.6/toolchain-mips/bin:/home/atcom-projects/ip-phone/toolchain/ifx-lxdb-1-2/hostutils/bin"
export CCACHE_PATH="/home/atcom-projects/ip-phone/toolchain/ifx-lxdb-1-2/gcc-3.3.6/toolchain-mips/bin-ccache"

if [ -n "$CC" ]; then
	echo "please clear you sys env"
	echo "you can exit tty and login again"
	echo ""
	exit 0
fi

export CC=mips-linux-gcc
export LD=mips-linux-ld
export AR=mips-linux-ar

rm output_ret/lib/* -rf
rm output_ret/include/* -rf
cd uart_protocol
make clean
make

cp -i libprotocol.a ../output_ret/lib/
cp -i app/app_proto.h ../output_ret/include/
cp -i link/link_proto.h ../output_ret/include/
cp -i tty/tty_proto.h ../output_ret/include/

make clean

cd ../libmxml-0.9.1
make clean
make distclean
autoreconf -i && chmod +x ./configure && ./configure --host=mips-linux
make
cp -i lib/libmxml.a ../output_ret/lib/
cp -i include/*.h ../output_ret/include/
make clean
make distclean
