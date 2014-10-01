#!/bin/sh
cd /usr/local

tar zxvf D620D.tar.gz
rm -rf D620D.tar.gz

mknod /dev/icc_user c 150 0
mknod /dev/icc_sam1 c 150 1
mknod /dev/icc_sam2 c 150 2
mknod /dev/icc_sam3 c 150 3

cd /usr/local/drv
insmod lcd.o
insmod printer.o
insmod magcard.o
insmod icc.o
insmod kbd.o
insmod control.o

ifconfig lo 127.0.0.1 up
./srv&

cd ../D620D
./D620D
