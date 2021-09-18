import copy
import struct
import itertools
import multiprocessing as mp

EEG_SESSION = {'tag': 'hep',
               'user_name': 'Hep',
               'sample_rate': 1000,
               'n_channels': 2,
               'n_samples_per_pack': 10,
               'gain': 1,
               'tcp_decimation':1}

GAME_SETTINGS = {'subfolder': "",
                 'duration': 10.0,
                 'volume': 100,
                 'complexity': 4}

CONNECTION_SETTINGS = {'ssid': "",
                       'password': ""}

EEG_SESSION_STR = b'Session'
GAME_SETTINGS_STR = b'GameSettings'
EEG_SAMPLE_HEADER = 0xACDC

NSV_STATE_TERM = 0
NSV_STATE_IDLE = 1
NSV_STATE_SESSION = 2
NSV_STATE_RECORD = 3

MCP_SAMPLE_GOOD = 239
MCP_SAMPLE_OLD = 555
MCP_SAMPLE_SKIPPED = 30
MCP_SAMPLE_RULE = 'I4iI'

NSV_SAMPLE_INDEX_ERROR = 0x1
NSV_BAD_SAMPLE = 0x7BADBAD7

#BCI_PATH = 'BCI'
PLAYLIST_PATH = 'playlist'
USERS_PATH = 'users'

SRV_TIMEOUT = 10
SRV_MSG_PORT = 7239
SRV_DATA_PORT = 8239
MSG_DELIM = b':'
MSG_END = b'\n\r'

def ConnectionSettings():
    return copy.deepcopy(CONNECTION_SETTINGS)

def GameSettings():
    return copy.deepcopy(GAME_SETTINGS)

def EegSession():
    return copy.deepcopy(EEG_SESSION)

def EegSamplePack(sample_list):
    n_samples = len(sample_list)
    n_channels = len(sample_list[0])
    header = (EEG_SAMPLE_HEADER << 16) | (n_channels << 8) | n_samples
    return struct.pack('!I' + str(n_samples * n_channels) + 'i',
                       *([header] + [int(i) for i in itertools.chain.from_iterable(sample_list)]))

def EegSampleFromMcpSample(mcp_sample):
	header = (EEG_SAMPLE_HEADER << 16) | 0x0400
	if mcp_sample[-1] == MCP_SAMPLE_SKIPPED:
		header |= NSV_SAMPLE_INDEX_ERROR
	return struct.pack('!I4i', header, *mcp_sample[1:5])

def EegSampleFromMcpSampleByte(mcp_sample):
	if mcp_sample[-4] == MCP_SAMPLE_SKIPPED:
		return b'\xac\xdc\x04\x01' + bytes(mcp_sample[7:3:-1]) + bytes(mcp_sample[11:7:-1]) + bytes(mcp_sample[15:11:-1]) + bytes(mcp_sample[19:15:-1])
	else:
		return b'\xac\xdc\x04\x00' + bytes(mcp_sample[7:3:-1]) + bytes(mcp_sample[11:7:-1]) + bytes(mcp_sample[15:11:-1]) + bytes(mcp_sample[19:15:-1])
