import socket
import struct
import json
import random
import time
import multiprocessing as mp
from nsv_def import *

SRV_TIMEOUT = 5
SRV_MSG_PORT = 7239
SRV_DATA_PORT = 8239
MSG_DELIM = b':'
MSG_END = b'\n\r'

MP_CTX = mp.get_context('spawn')

def data_process(flag):
    dataSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
    dataSock.bind(('', SRV_DATA_PORT))
    dataSock.listen(1)
    clientSock, clientAddr = dataSock.accept()
    while flag.value != 0:
        if flag.value == 1:
            data = [int(random.random() * 1000)] * 4
            clientSock.send(EegSamplePack(data))
            time.sleep(0.01)
        else:
            time.sleep(1)
    clientSock.close()
    dataSock.close()

class NeuroslaveTcpServer:

    COMMANDS = {b'Start': lambda s, msg: s.session_start(msg),
                b'Stop': lambda s, msg: s.session_stop(msg),
                b'Set': lambda s, msg: s.process_session(msg)}

    def __init__(self):
        self.servSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.session = EegSession()
        self.srv_running = False
        self.clientSock = None
        self.dataServer = None
        self.data_srv_running = MP_CTX.Value('i', 0)

    def __del__(self):
        self.stop()

    def stop(self):
        self.srv_running = False
        try:
            self.clientSock.close()
            self.servSock.close()
            self.data_srv_running.value = 0
            self.dataServer.join()
            self.dataServer.kill()
        except Exception as e:
            print(e)

    def send_msg(self, text):
        self.clientSock.send(b'Message:' + bytes(text, encoding = 'UTF-8') + MSG_END)
        print(text)

    def send_session(self):
        self.clientSock.send(EEG_SESSION_STR + MSG_DELIM +
                             bytes(json.dumps(self.session), encoding = 'UTF-8') + MSG_END)

    def process_cmd(self, msg):
        cmd = msg.split(MSG_DELIM)[0]
        if (cmd.find(MSG_END) > 0):
            cmd = cmd[:cmd.find(MSG_END)]
        if cmd in self.COMMANDS.keys():
            if not self.COMMANDS[cmd](self, msg[msg.find(MSG_DELIM) + len(MSG_DELIM):]):
                self.send_msg('Command failed')
            else:
                self.send_msg('Command accepted')
        else:
            self.send_msg('Wrong command')

    def session_start(self, msg):
        self.send_session()
        self.data_srv_running.value = 1
        return True

    def session_stop(self, msg):
        self.data_srv_running.value = 2
        self.send_msg('Stopped')
        return True

    def process_session(self, msg):
        json_msg = msg[msg.find(MSG_DELIM) + len(MSG_DELIM):]
        if len(json_msg) == 0: return False
        if msg[:msg.find(MSG_DELIM)] != EEG_SESSION_STR: return False
        try:
            new_session = json.loads(json_msg.decode('UTF-8'))
            if new_session.keys() == self.session.keys():
                self.session = new_session
                print(self.session)
                return True
            return False
        except Exception as e:
            print(e)
            return False
            

    def run(self):
        self.servSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.servSock.settimeout(SRV_TIMEOUT)
        self.servSock.bind(('', SRV_MSG_PORT))
        self.servSock.listen(1)
        self.srv_running = True
        while self.srv_running:
            try:
                self.clientSock, self.clientAddr = self.servSock.accept()
                self.clientSock.settimeout(SRV_TIMEOUT)
                print("Accepted")
                self.data_srv_running.value = 2
                self.dataServer = MP_CTX.Process(target = data_process, args = (self.data_srv_running,))
                self.dataServer.start()
                print("Data srv started")
                while self.srv_running:
                    try:
                        clientRequest = self.clientSock.recv(1024)
                        print(clientRequest)
                        if not clientRequest:
                            print("None recevied")
                        self.process_cmd(clientRequest)
                    except Exception as e:
                        print("Recv error", e)   
                self.clientSock.close()
                self.data_srv_running.value = 0
                self.dataServer.join()
                self.dataServer.kill()
                print("Client closed")
            except Exception as e:
                print("Accept error", e)
        self.servSock.close()
        print("Server closed")


if __name__ == "__main__":
    nsvsrv = NeuroslaveTcpServer()
    nsvsrv.run()
