#!/bin/bash
# build
gcc wmain.c wave_music.c -std=c11 -pthread -lrt -lasound -lpthread -lportaudio -o wmain 
jack_control start
./wmain