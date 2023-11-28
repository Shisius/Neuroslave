#!/bin/bash
# tmp dir
if [ ! -d "tmp" ]
then
	echo "Creating tmp dir"
	mkdir tmp
fi
cd tmp/
# run cmake
cmake ../ 
make -j4
echo "Project built"

arm-none-eabi-objcopy -O binary neuroslave_sea.elf neuroslave.bin
dfu-suffix -a neuroslave.bin
