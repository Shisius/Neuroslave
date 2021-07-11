#!/bin/bash
# common
sudo apt-get install python3 python3-pip g++ cmake git
# portaudio
sudo apt-get install libasound-dev
cd ~
mkdir LIBS
cd LIBS
git clone https://github.com/PortAudio/portaudio.git
cd portaudio
./configure && make
sudo make install
# wiring pi
