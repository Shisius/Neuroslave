import copy
import struct

EEG_SESSION = {'tag': 'hep',
               'sample_rate': 1000,
               'n_channels': 4}

EEG_SESSION_STR = b'EegSession'
EEG_SAMPLE_HEADER = 0xACDC

EEG_CMD = (b'Start', b'Stop', b'Set')

def EegSession():
    return copy.deepcopy(EEG_SESSION)

def EegSamplePack(sample_list):
    n_samples = len(sample_list)
    header = (EEG_SAMPLE_HEADER << 16) | (n_samples << 8)
    return struct.pack('!I' + str(n_samples) + 'i',
                       *([header] + [int(i) for i in sample_list]))

