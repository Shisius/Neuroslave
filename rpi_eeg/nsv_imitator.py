import socket
import struct
import json
import random
import time
import math
import os
import multiprocessing as mp
import subprocess
import threading
import signal
import random
import wireless
from PyAccessPoint import pyaccesspoint
from nsv_def import *

MP_CTX = mp.get_context('spawn')

def data_process(pipe_con, flag):
    dataSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
    dataSock.settimeout(SRV_TIMEOUT)
    dataSock.bind(('', SRV_DATA_PORT))
    dataSock.listen(1)
    while flag.value != NSV_STATE_TERM:
        try:
            clientSock, clientAddr = dataSock.accept()
            clientSock.settimeout(SRV_TIMEOUT)
            counter = 0
            session = EegSession()
            while flag.value != NSV_STATE_TERM:
                if pipe_con.poll():
                    session = pipe_con.recv()
                while flag.value >= NSV_STATE_SESSION:
                    data = [int(math.sin(counter * 2 * math.pi / session['sample_rate']) * 100 * session['gain'])] * session['n_channels']
                    if ((counter % session['tcp_decimation']) == 0):
                        try:
                            clientSock.send(EegSamplePack(data))
                        except Exception as e:
                            pass
                    counter += 1
                    time.sleep(0.001)
                time.sleep(0.5)
            clientSock.close()
        except Exception as e:
            print("Accept error", e)
    dataSock.close()

class NeuroslaveImitator:

    COMMANDS = {b'TurnOn': lambda s, msg: s.session_start(msg),
                b'TurnOff': lambda s, msg: s.session_stop(msg),
                b'Set': lambda s, msg: s.process_set(msg),
                b'User': lambda s, msg: s.choose_user(msg),
                b'Choose': lambda s, msg: s.choose_music(msg),
                b'Record': lambda s, msg: s.record_start(msg),
                b'Stop': lambda s, msg: s.record_stop(msg),
                b'Game': lambda s, msg: s.guess_melody(msg)}

    def __init__(self):
        self.servSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.session = EegSession()
        self.game_settings = GameSettings()
        self.srv_running = False
        self.clientSock = None
        #self.dataHandler = None
        self.dataSource = None
        self.state = MP_CTX.Value('i', NSV_STATE_IDLE)
        self.data_pipe_ctl, self.data_pipe_src = MP_CTX.Pipe()
        self.choosen_music = ""
        self.user = self.get_users()[0]
        self.access_point = pyaccesspoint.AccessPoint()

        self.timer = None

        signal.signal(signal.SIGINT, self.server_stop)

    def __del__(self):
        self.terminate()

    def terminate(self):
        print('Termination')
        self.srv_running = False
        try:
            #self.clientSock.close()
            #self.servSock.close()
            self.state.value = NSV_STATE_TERM
            #self.dataHandler.join()
            #self.dataHandler.kill()
            self.dataSource.join()
            self.dataSource.kill()
        except Exception as e:
            print(e)

    def create_network(self):
        self.access_point.start()

    def check_network(self):
        ip_str = subprocess.check_output(['hostname', '-I'])
        if len(ip_str.split(b'.')) > 3:
            return True
        else:
            return False

    def connect_to_network(self, con_settings):
        if self.access_point.is_running():
            self.access_point.stop()
        wire = wireless.Wireless()
        if wire.connect(ssid = con_settings['ssid'], password = con_settings['password']):
            return True
        self.access_point.start()
        return False

    def server_stop(self, sig_arg1, sig_arg2):
        self.srv_running = False

    def send_msg(self, text):
        self.clientSock.send(b'Message:' + bytes(text, encoding = 'UTF-8') + MSG_END)
        print(text)

    def send_session(self):
        self.clientSock.send(EEG_SESSION_STR + MSG_DELIM +
                             bytes(json.dumps(self.session), encoding = 'UTF-8') + MSG_END)

    def get_users(self):
        return os.listdir(USERS_PATH)

    def send_users(self):
        msg = b"Users" + MSG_DELIM + bytes(json.dumps(self.get_users()), encoding = 'UTF-8') + MSG_END
        self.clientSock.send(msg)

    def get_playlist(self):
        return os.listdir(USERS_PATH + '/' + self.user + '/' + PLAYLIST_PATH)

    def send_playlist(self):
        msg = b"Playlist" + MSG_DELIM + bytes(json.dumps(self.get_playlist()), encoding = 'UTF-8') + MSG_END
        self.clientSock.send(msg)

    def send_record_finished(self):
        self.clientSock.send(b'Record' + MSG_DELIM + b'Finished' + MSG_END)

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
        self.data_pipe_ctl.send(self.session)
        self.state.value = NSV_STATE_SESSION
        return True

    def session_stop(self, msg):
        self.state.value = NSV_STATE_IDLE
        self.send_msg('Stopped')
        return True

    def process_set(self, msg):
        json_msg = msg[msg.find(MSG_DELIM) + len(MSG_DELIM):]
        if len(json_msg) == 0: return False
        if msg[:msg.find(MSG_DELIM)] == EEG_SESSION_STR:
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
        if msg[:msg.find(MSG_DELIM)] == GAME_SETTINGS_STR:
            try:
                new_game_settings = json.loads(json_msg.decode('UTF-8'))
                if new_game_settings.keys() == self.game_settings.keys():
                    self.game_settings = new_game_settings
                    print(self.game_settings)
                    return True
                return False
            except Exception as e:
                print(e)
                return False
        return False

    def choose_user(self, msg):
        user_name = msg[:msg.find(MSG_END)].decode('UTF-8')
        if len(user_name) <= 0:
            self.send_users()
            return True
        if user_name in os.listdir(USERS_PATH):
            self.user = user_name
            self.send_msg(user_name + " choosen")
            return True
        self.send_msg(user_name + " - Wrong user name")
        return False

    def choose_music(self, msg):
        file_name = msg[:msg.find(MSG_END)].decode('UTF-8')
        if len(file_name) <= 0:
            self.send_playlist()
            return True
        if file_name in self.get_playlist():
            self.choosen_music = file_name
            self.send_msg(file_name + " choosen")
            return True
        self.send_msg(file_name + " - Wrong file name")
        return False

    def record_start(self, msg, dur = 10):
        self.timer = threading.Timer(dur, self.send_record_finished)
        self.timer.start()
        return True

    def record_stop(self, msg):
        self.timer.cancel()
        self.send_record_finished()
        return True

    def game_start(self):
        self.timer = threading.Timer(self.game_settings['duration'], self.game_finished)
        self.timer.start()

    def game_finished(self):
        complexity = min(self.game_settings['complexity'], len(self.get_playlist()))
        i_guess = 0
        guess_list = []
        while i_guess < complexity:
            new_guess = self.get_playlist()[random.randrange(len(self.get_playlist()))]
            if not (new_guess in guess_list):
                guess_list += [new_guess]
                i_guess += 1
        msg = b"Game" + MSG_DELIM + bytes(json.dumps(guess_list), encoding = 'UTF-8') + MSG_END
        self.clientSock.send(msg)

    def guess_melody(self, msg):
        file_name = msg[:msg.find(MSG_END)].decode('UTF-8')
        if len(file_name) <= 0:
            self.choosen_music = self.get_playlist()[random.randrange(len(self.get_playlist()))]
            self.game_start()
            return True
        if file_name in self.get_playlist():
            if file_name == self.choosen_music:
                self.send_msg(self.user + " wins!")
            else:
                self.send_msg(self.user + " loses(")
            self.clientSock.send(b'Game' + MSG_DELIM + bytes(self.choosen_music, encoding = 'UTF-8') + MSG_END)
            print("Right: " + self.choosen_music)
            return True
        self.send_msg(file_name + " - Wrong file name")
        return False

    def start_data_source(self):
        self.dataSource = MP_CTX.Process(target = data_process, args = (self.data_pipe_src, self.state))
        self.dataSource.start()
        print("Data source started")

    def run(self):
        self.servSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.servSock.settimeout(SRV_TIMEOUT)
        self.servSock.bind(('', SRV_MSG_PORT))
        self.servSock.listen(1)
        self.srv_running = True
        
        self.state.value = NSV_STATE_IDLE
        self.start_data_source()
        
        while self.srv_running:
            try:
                self.clientSock, self.clientAddr = self.servSock.accept()
                self.clientSock.settimeout(SRV_TIMEOUT)
                print("Accepted")
                while self.srv_running:
                    try:
                        clientRequest = self.clientSock.recv(1024)
                        print(clientRequest)
                        if not clientRequest:
                            print("None recevied")
                        self.process_cmd(clientRequest)
                    except socket.error as e:
                        print("Recv error: ", e)
                        if isinstance(e.args, tuple):
                            if len(e.args) > 0:
                                if e.args[0] == socket.errno.EPIPE:
                                    break
                    except Exception as e:
                        print("Another Recv error", e)
                self.clientSock.close()
                print("Client closed")
            except Exception as e:
                print("Accept error", e)
        self.servSock.close()
        print("Server closed")
        self.terminate()
        print("Terminated")

if __name__ == "__main__":
    nsvsrv = NeuroslaveImitator()
    nsvsrv.run()
