#!/bin/bash

echo "\$1 is $1"
if [ "$1" != "" ]; then
no=$1
else
no=100
fi
echo "obase=2;$no" | bc
echo "obash=10;ibash=2;$no" | bc

