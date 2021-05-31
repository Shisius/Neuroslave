import socket
import struct
import json
import random
import time
import math
import os
import multiprocessing as mp
import threading
import signal
from nsv_def import *
from nsv_data_handler import *
from ganglion_spi import *

MP_CTX = mp.get_context('spawn')

def data_process(flag):
    dataSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
    dataSock.bind(('', SRV_DATA_PORT))
    dataSock.listen(1)
    clientSock, clientAddr = dataSock.accept()
    counter = 0
    while flag.value != 0:
        if flag.value == 1:
            counter += 1
            data = [int(math.sin(counter * 2 * math.pi * 0.001) * 1000)] * 4
            try:
                clientSock.send(EegSamplePack(data))
            except Exception as e:
                pass
            time.sleep(0.001)
        else:
            time.sleep(1)
    clientSock.close()
    dataSock.close()

class NeuroslaveTcpServer:

    COMMANDS = {b'TurnOn': lambda s, msg: s.session_start(msg),
                b'TurnOff': lambda s, msg: s.session_stop(msg),
                b'Set': lambda s, msg: s.process_session(msg),
                b'Choose': lambda s, msg: s.choose_music(msg),
                b'Record': lambda s, msg: s.record_start(msg),
                b'Stop': lambda s, msg: s.record_stop(msg)}

    def __init__(self):
        self.servSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.session = EegSession()
        self.srv_running = False
        self.clientSock = None
        self.dataHandler = None
        self.dataSource = None
        self.state = MP_CTX.Value('i', NSV_STATE_IDLE)
        self.data_pipe_ctl, self.data_pipe_src = MP_CTX.Pipe()
        self.choosen_music = ""

        self.timer = None

        signal.signal(signal.SIGINT, self.server_stop)

    def __del__(self):
        self.terminate()

    def terminate(self):
        self.srv_running = False
        try:
            self.clientSock.close()
            self.servSock.close()
            self.state = NSV_STATE_TERM
            self.dataHandler.join()
            self.dataHandler.kill()
            self.dataSource.join()
            self.dataSource.kill()
        except Exception as e:
            print(e)

    def server_stop(self, sig_arg1, sig_arg2):
        self.srv_running = False

    def send_msg(self, text):
        self.clientSock.send(b'Message:' + bytes(text, encoding = 'UTF-8') + MSG_END)
        print(text)

    def send_session(self):
        self.clientSock.send(EEG_SESSION_STR + MSG_DELIM +
                             bytes(json.dumps(self.session), encoding = 'UTF-8') + MSG_END)

    def send_playlist(self):
        msg = b"Playlist" + MSG_DELIM + bytes(json.dumps(os.listdir(PLAYLIST_PATH)), encoding = 'UTF-8') + MSG_END
        self.clientSock.send(msg)

    def send_record_finished(self):
        self.clientSock.send(b'Record' + MSG_DELIM + b'Finished' + MSG_END)
##        try:
##            self.timer.join()
##        except:
##            pass

    def process_cmd(self, msg):
        cmd = msg.split(MSG_DELIM)[0]
        if (cmd.find(MSG_END) > 0):
            cmd = cmd[:cmd.find(MSG_END)]
        if (msg.find(MSG_DELIM) > 0):
            payload = msg[msg.find(MSG_DELIM) + len(MSG_DELIM):]
        else:
            payload = msg[len(cmd):]
        if cmd in self.COMMANDS.keys():
            if not self.COMMANDS[cmd](self, payload):
                self.clientSock.send(cmd + MSG_DELIM + b'Failed' + MSG_END)
                print(cmd + b":Failed")
            else:
                self.clientSock.send(cmd + MSG_DELIM + b'Accepted' + MSG_END)
                print(cmd + b":Accepted")
        else:
            self.send_msg('Wrong command')

    def session_start(self, msg):
        self.send_session()
        self.state.value = NSV_STATE_SESSION
        self.data_pipe_ctl.send(self.session)
        return True

    def session_stop(self, msg):
        self.state.value = NSV_STATE_IDLE
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

    def choose_music(self, msg):
        file_name = msg[:msg.find(MSG_END)].decode('UTF-8')
        if len(file_name) <= 0:
            self.send_playlist()
            return True
        if file_name in os.listdir(PLAYLIST_PATH):
            self.choosen_music = file_name
            self.send_msg(file_name + " choosen")
            return True
        self.send_msg(file_name + " - Wrong file name")
        return False

    def record_start(self, msg):
        #self.timer = threading.Timer(10, self.send_record_finished)
        #self.timer.start()
        self.state.value = NSV_STATE_RECORD
        return True

    def record_stop(self, msg):
        #self.timer.cancel()
        self.state.value = NSV_STATE_SESSION
        self.send_record_finished()
        return True

    def start_data_handler(self):
        self.dataHandler = MP_CTX.Process(target = nsv_data_handler_process, args = (self.data_pipe_ctl, self.state))
        self.dataHandler.start()
        print("Data handler started")

    def start_data_source(self):
        self.dataSource = MP_CTX.Process(target = ganglion_spi_process, args = (self.data_pipe_src, self.state))
        self.dataSource.start()
        print("Data source started")

    def run(self):
        self.servSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        #self.servSock.settimeout(SRV_TIMEOUT)
        self.servSock.bind(('', SRV_MSG_PORT))
        self.servSock.listen(1)
        self.srv_running = True
        while self.srv_running:
            #try:
            self.clientSock, self.clientAddr = self.servSock.accept()
            #self.clientSock.settimeout(SRV_TIMEOUT)
            print("Accepted")
            self.state.value = NSV_STATE_IDLE
            self.start_data_handler()
            self.start_data_source()
            while self.srv_running:
                #try:
                clientRequest = self.clientSock.recv(1024)
                print(clientRequest)
                if not clientRequest:
                    print("None recevied")
                self.process_cmd(clientRequest)
                # except socket.error as e:
                #     print("Recv error: ", e)
                #     if isinstance(e.args, tuple):
                #         if len(e.args) > 0:
                #             if e.args[0] == socket.errno.EPIPE:
                #                 break
                # except Exception as e:
                #     print("Another Recv error", e)
            self.state.value = NSV_STATE_TERM
            self.dataHandler.join()
            self.dataHandler.kill()
            self.dataSource.join()
            self.dataSource.kill()
            self.clientSock.close()
            print("Client closed")
            #except Exception as e:
            #    print("Accept error", e)
        self.terminate()
        print("Server closed")


if __name__ == "__main__":
    nsvsrv = NeuroslaveTcpServer()
    nsvsrv.run()
