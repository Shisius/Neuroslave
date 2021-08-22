#!/bin/bash
g++ nsvtest.cpp neuroslave_ctl.cpp ../communication/serverTCP/tcpserver.cpp ../ganglion_comm/ganglion_comm.cpp -I../third_party/rapidjson -Idefinitions -I../common_tools -I../communication/serverTCP -I../ganglion_comm -DNSV_DUMMY_SOURCE -Wall -pthread -o nsvtest
