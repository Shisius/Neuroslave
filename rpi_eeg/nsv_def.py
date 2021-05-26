import copy
import struct
import multiprocessing as mp

EEG_SESSION = {'tag': 'hep',
               'sample_rate': 1000,
               'n_channels': 4}

EEG_SESSION_STR = b'EegSession'
EEG_SAMPLE_HEADER = 0xACDC

NSV_STATE_TERM = 0
NSV_STATE_IDLE = 1
NSV_STATE_SESSION = 2
NSV_STATE_RECORD = 3

MCP_SAMPLE_GOOD = 239
MCP_SAMPLE_OLD = 555
MCP_SAMPLE_SKIPPED = 30

PLAYLIST_PATH = 'playlist'

SRV_TIMEOUT = 5
SRV_MSG_PORT = 7239
SRV_DATA_PORT = 8239
MSG_DELIM = b':'
MSG_END = b'\n\r'



def EegSession():
    return copy.deepcopy(EEG_SESSION)

def EegSamplePack(sample_list):
    n_samples = len(sample_list)
    header = (EEG_SAMPLE_HEADER << 16) | (n_samples << 8)
    return struct.pack('!I' + str(n_samples) + 'i',
                       *([header] + [int(i) for i in sample_list]))

