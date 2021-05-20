import spidev
import struct
import time
import multiprocessing as mp
import numpy as np
import sys
import os
from nsv_def import *

GANG_SPIS_MODE = 0
GANG_SPIS_HZ = 10000000

GANG_CMD_SAMPLE = 0x1
GANG_CMD_START = 0x2
GANG_SAMPLE_SIZE = 24
GANG_SAMPLE_RULE = 'I4iI'

class GanglionSpiComm:

    def __init__(self, pipe_con = None, state_flag = None):
        #set_priority="sudo renice -n -20 -p "
        #mypid = os.getpid()
        #ret = os.system(set_priority+str(mypid))
        #sys.setcheckinterval(1000);
        # SPI
        self.spi = spidev.SpiDev(0, 0)
        self.spi.mode = GANG_SPIS_MODE
        self.spi.max_speed_hz = GANG_SPIS_HZ
        # Interaction
        self.pipe = pipe_con
        self.state = state_flag
        # Session parameters
        self.session = EegSession()
        # State
        self.expected_index = 0
        self.sample_counter = 0

    def __del__(self):
        self.spi.close()

    def run(self):
        while self.state.value != NSV_STATE_TERM:
            self.session = pipe.recv()
            self.setup()
            if self.state.value == NSV_STATE_SESSION:
                self.start()

    def setup(self):
        pass

    def start(self):
        self.sample_counter = 0
        start_sample = self.get_sample(GANG_CMD_START)
        self.expected_index = start_sample[0]
        while self.state.value >= NSV_STATE_SESSION:
            sample_list = self.get_sample()            
            # check
            if sample_list[-1] == MCP_SAMPLE_GOOD:
                if not self.check(sample_list[0]):
                    sample_list[-1] == MCP_SAMPLE_BAD
                # send
                self.pipe.send(sample_list)
                self.sample_counter += 1

    def check(self, index):
        if index != self.expected_index:
            print("expected:", self.expected_index, " real: ", index)
            return False
        self.expected_index += 1
        return True

    def get_sample(self, cmd = GANG_CMD_SAMPLE):
        #self.spi.writebytes([cmd])
        self.spi.xfer([cmd])
        b_data = self.spi.readbytes(GANG_SAMPLE_SIZE)
        return struct.unpack(GANG_SAMPLE_RULE, bytes(b_data))
        
def ganglion_spi_process(pipe, state)
    gsc = GanglionSpiComm(pipe, state)
    gsc.run()
