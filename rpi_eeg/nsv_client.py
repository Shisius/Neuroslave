import socket
import struct

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
s.connect(('192.168.43.106', 8239))
while (True):
    ac = s.recv(1)
    if ac == b'\xac':
        dc = s.recv(1)
        if dc == b'\xdc':
            l = s.recv(1)
            if l == b'\x04':
                state = s.recv(1)
                data = struct.unpack('!4i', s.recv(16))
                print(state, data)
        else:
            print('wrong ', dc)
s.close()
