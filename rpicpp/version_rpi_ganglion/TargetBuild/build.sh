#!/bin/bash
# tmp dir
if [ ! -d "tmp" ]
then
	echo "Creating tmp dir"
	mkdir tmp
fi
cd tmp/
# run cmake
echo "Running cmake"
cmake -DNSV_BUILD_FOLDER="${PWD}/.." -j4 ../../ 
echo "Building project"
make -j4
echo "Project built"
