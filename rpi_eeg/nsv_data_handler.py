import socket
import time
import pickle
import multiprocessing as mp

from nsv_def import *


class NeuroslaveEegDataHandler:

    def __init__(self, pipe_con = None, state_flag = None):
        # Interaction
        self.pipe = pipe_con
        self.state = state_flag
        # Data server
        self.dataSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.clientSock = None
        self.record = []
        self.record_lock = False

    def save_record(self):
        f = open("sample.eeg", 'wb')
        pickle.dump(self.record, f)
        f.close()

    def run(self):
        self.dataSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        #self.dataSock.settimeout(SRV_TIMEOUT)
        self.dataSock.bind(('', SRV_DATA_PORT))
        self.dataSock.listen(1)
        while self.state.value != NSV_STATE_TERM:
            #try:
            self.clientSock, clientAddr = self.dataSock.accept()
            #self.clientSock.settimeout(SRV_TIMEOUT)
            while self.state.value != NSV_STATE_TERM:
                if self.pipe != None and self.state.value >= NSV_STATE_SESSION:
                    if self.pipe.poll():
                        sample = self.pipe.recv()
                        if self.state.value == NSV_STATE_SESSION:
                            if self.record_lock:
                                self.save_record()
                                self.record = []
                                self.record_lock = False
                            self.clientSock.send(EegSampleFromMcpSampleByte(sample))
                        elif self.state.value == NSV_STATE_RECORD:
                            if not self.record_lock:
                                self.record = [sample]
                                self.record_lock = True
                            else:
                                self.record += [sample]
                        continue
                time.sleep(0.01)
            self.clientSock.close()
            #except Exception as e:
            #    print(e)
        self.dataSock.close()


def nsv_data_handler_process(pipe, state):
    ndh = NeuroslaveEegDataHandler(pipe, state)
    ndh.run()
    
