import socket
import requests
import struct
import json
import re
import time
from collections import deque

DEFAULT_SAMPLE_RATE = 1600
DEFAULT_TCP_PORT = 9239
DEFAULT_GANGLION_IP = '192.168.1.248'
DEFAULT_USER_IP = '192.168.1.121'
DEFAULT_TCP_SETTINGS = {'port': DEFAULT_TCP_PORT,
                        'ip': DEFAULT_USER_IP,
                        'delimiter': True,
                        'latency': 20000,
                        'sample_numbers': True,
                        'timestamps': True}

DEFAULT_CHANNELS_POSITION = {0: 'channel_broca_right',
                             1: 'channel_wernicke_right',
                             3: 'channel_broca_left',
                             2: 'channel_wernicke_left'}

SOUND_EEG_DATA  =  {'sample_rate': 0,
                    'channel_broca_right': [],
                    'channel_broca_left': [],
                    'channel_wernicke_right': [],
                    'channel_wernicke_left': [],
                    'timestamps': []}
EEG_EXT = '.eeg'

def sample24toInt(sample_bytes):
    return (sample_bytes[0] << 16) + (sample_bytes[1] << 8) + sample_bytes[2]

class GanglionControl:

    SPS_CMD = {25600: b'0',
               12800: b'1',
               6400:  b'2',
               3200:  b'3',
               1600:  b'4',
               800:   b'5',
               400:   b'6',
               200:   b'7'}
    RAW33_HEADER = 0xA0
    RAW33_LENGTH = 33
    RAW33_HEADER_B = b'\xA0'
    N_EEG_CHANNELS = 4
    PARSE_FOOTER = {0xc0: lambda s: s.parse_footer(),
                    0xc1: lambda s: s.parse_footer(),
                    0xc2: lambda s: s.parse_footer(),
                    0xc3: lambda s: s.parse_footer(),
                    0xc4: lambda s: s.parse_footer(),
                    0xc5: lambda s: s.parse_footer(),
                    0xc6: lambda s: s.parse_footer()}  

    def __init__(self):
        # Settings
        self.sample_rate = DEFAULT_SAMPLE_RATE
        self.tcp_settings = DEFAULT_TCP_SETTINGS
        self.tcp_settings['port'] = DEFAULT_TCP_PORT
        self.tcp_settings['ip'] = DEFAULT_USER_IP
        self.ganglion_ip = DEFAULT_GANGLION_IP
        self.channels_position = dict(DEFAULT_CHANNELS_POSITION)
        self.eeg_buffer_depth = 1024
        # Socket
        self.tcp_host_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.tcp_remote_sock = None
        # Data
        self.eeg_buffer = dict(zip(self.channels_position.keys(),
                                   [deque([0]*self.eeg_buffer_depth, maxlen = self.eeg_buffer_depth)] * len(self.channels_position.keys())))
        self.eeg_times_buffer = deque([0]*self.eeg_buffer_depth, maxlen = self.eeg_buffer_depth)
        self.eeg_record = dict(SOUND_EEG_DATA)
        # State
        self.reading = False
        self.recording = False
        self.expected_sample_index = 0
        self.bin_pack = b''
        self.http_response = ''

    def setup_tcp(self):
        self.tcp_host_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.tcp_host_sock.bind(('', self.tcp_settings['port']))
        self.tcp_host_sock.listen(1)
        self.set_sample_rate_tcp(self.sample_rate)
        self.http_response += requests.post('http://' + self.ganglion_ip + '/tcp', data = json.dumps(self.tcp_settings))
        self.tcp_remote_sock, tcp_remote_address = self.tcp_host_sock.accept()

    def start_tcp(self):
        self.expected_sample_index = 0
        self.reading = True
        self.http_response = requests.get('http://' + self.ganglion_ip + '/stream/start')

    def stop_tcp(self):
        self.reading = False
        self.http_response = requests.get('http://' + self.ganglion_ip + '/stream/stop')

    def close_tcp(self):
        self.tcp_remote_sock.close()

    def set_sample_rate_tcp(self, sample_rate):
        if sample_rate in self.SPS_CMD.keys():
            self.http_response = requests.post('http://' + self.ganglion_ip + '/command',
                                               data = b'~' + self.SPS_CMD[sample_rate])
            for new_sample_rate in re.findall(r'\d+', self.http_response):
                if int(new_sample_rate) in self.SPS_CMD.keys():
                    self.sample_rate = int(new_sample_rate)

    def read_raw33(self):
        self.bin_pack = self.tcp_remote_sock.read(self.RAW33_LENGTH)

    def find_read_raw33(self):
        while self.tcp_remote_sock.read(1) != self.RAW33HEADER_B:
            pass
        self.bin_pack = self.RAW33HEADER_B + self.tcp_remote_sock.read(self.RAW33_LENGTH - 1)

    def parse_raw33(self):
        if self.bin_pack[0] != self.RAW33HEADER:
            print('Wrong raw33 pack header')
            return -1
        current_sample_index = self.bin_pack[1]
        if current_sample_index != self.expected_sample_index:
            print('Expected = ' + str(self.expected_sample_index) + ' Current = ' + current_sample_index)
        self.expected_sample_index = (current_sample_index + 1) & 0xFF
        # Parse EEG data
        for ch in range(self.N_EEG_CHANNELS):
            sample = sample24toInt(self.bin_pack[2 + 3*ch:5 + 3*ch])
            self.eeg_buffer[ch].append(sample)
            if self.recording:
                self.eeg_record[self.channels_position[ch]].append(sample)

    def reading_process_tcp(self):
        while self.reading:
            self.find_read_raw33()
            self.parse_raw33()
        
    def parse_footer(self):
        pass
    
