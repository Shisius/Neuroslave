import socket

from nsv_def import *


class NeuroslaveEegDataHandler:

    def __init__(self, pipe_con = None, state_flag = None):
        # Interaction
        self.pipe = pipe_con
        self.state = state_flag
        # Data server
        self.dataSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.clientSock = None

    def run(self):
        self.dataSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.dataSock.settimeout(SRV_TIMEOUT)
        self.dataSock.bind(('', SRV_DATA_PORT))
        self.servSock.listen(1)
        while self.state != NSV_STATE_TERM:
            pass #try:
                
    
