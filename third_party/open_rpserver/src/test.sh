#!/bin/sh

export PATH="$PATH:/home/atcom-projects/ip-phone/toolchain/ifx-lxdb-1-2/gcc-3.3.6/toolchain-mips/bin:/home/atcom-projects/ip-phone/toolchain/ifx-lxdb-1-2/hostutils/bin"
 export CCACHE_PATH="/home/atcom-projects/ip-phone/toolchain/ifx-lxdb-1-2/gcc-3.3.6/toolchain-mips/bin-ccache"


 ./configure --host=mips-linux --enable-plugin=/home/wyj/at960/ip-phone/rainbow/rainbow4/ifx-com-linux/source/user/opensource/open_newp     ppoe CPPFLAGS=-I/home/wyj/at960/ip-phone/rainbow/rainbow4/ifx-com-linux/source/user/opensource/open_newpppoe
