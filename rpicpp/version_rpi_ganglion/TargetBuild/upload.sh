#!/bin/bash

sshpass -p tmc2209ESP#@ scp -r ../../version_rpi_ganglion pi@192.168.0.101:nsvcpp/

sshpass -p tmc2209ESP#@ scp -r ../../../libc pi@192.168.0.101:
