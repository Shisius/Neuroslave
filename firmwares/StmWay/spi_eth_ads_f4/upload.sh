#!/bin/bash

dfu-util -a 0 -i 0 -s 0x08000000:leave -D tmp/neuroslave.bin 
