#!/bin/bash
# clean
# rm wmain
killall wmain
# build
gcc wmain.c wave_music.c -Ofast -std=c11 -lrt -lasound -lpthread -lportaudio -o wmain 
#jack_control start
./wmain