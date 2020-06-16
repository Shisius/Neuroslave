# Signal Monitor

import sys
PyToolsPath = "/home/shisius/Projects/PyTools"
if not(PyToolsPath in sys.path):
    sys.path.append(PyToolsPath)

import socket
import struct
import drawer
import numpy as np
from tkinter import *
from time import sleep
from collections import deque
from connector import *

def ord_arr(arr):
    return [ord(s) for s in arr]

class NeuroMessage:

    BEGIN_SYMB = b'$'
    END_SYMB = b'END\x00'

    def __init__(self):
        self.signal_quality = 0
        self.attention = 0
        self.meditation = 0
        self.time_points = []
        self.signal_samples = []
        self.n_samples = 10
        self.raw_rule = self.rawdata_structrule()
        self.length = 52

    def take_signal(self):
        raw = self.signal_samples
        self.signal_samples = []
        #print(raw)
        self.time_points = []
        return raw

    def bindatachk(self, bindata):
        return ((bindata[0:len(self.BEGIN_SYMB)] == self.BEGIN_SYMB) and
            (bindata[-len(self.END_SYMB):] == self.END_SYMB))

    def rawdata_structrule(self):
        rule = '<' + 'hH' * self.n_samples
        return rule

    def parse(self, bindata):
        if not (self.bindatachk(bindata)):
            print("CHK FAILED")
            return
        if (bindata[1] == ord(b'Q')):
            self.signal_quality = bindata[2]
        else: print("No Q")
        if (bindata[3] == ord(b'A')):
            self.attention = bindata[4]
        else: print("No A")
        if (bindata[5] == ord(b'M')):
            self.meditation = bindata[6]
        else: print("No M")
        if (bindata[7] == ord(b'S')):
            raw = struct.unpack(self.raw_rule, bindata[8:-4])
            self.signal_samples = raw[0::2]
            self.time_points = raw[1::2]
        else: print("No S")
            
        

class UDPServer:

    def __init__(self, port):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.port = port
        self.socket.bind(('', port))
        self.msg = NeuroMessage()

    def read_msg(self):
        bindata = self.socket.recv(1024)
        if len(bindata) == self.msg.length:
            #print(bindata)
            self.msg.parse(bindata)
        else: print("WRONG LENGTH")
        #print("Q: %d, A: %d, M: %d" % (self.msg.signal_quality,
        #                               self.msg.attention, self.msg.meditation))
        

    def start(self):
        print("UDP started")
        while True:
            self.read_msg()
            sleep(0.1)
        print("UDP finished")

#class ValueLabel(Label):

 #   def __init__(self, 
        

class ControlFrame(Frame):

    def __init__(self, tk = None, **kwargs):
        Frame.__init__(self, master = tk, **kwargs)
        self.tk = tk
        self.n_objects = 4
        self.object_height = 10
        self.object_width = 10
        self.start_button = drawer.StartStopButton(start_func = self.tk.start,
                                                   stop_func = self.tk.stop,
                                                   master = self,
                                                   width = self.object_width,
                                                   height = self.object_height
                                                   )
        self.start_button.grid(row = 0, column = 0)
        # Labels
        self.label_dw = 3
        self.quality_text = "Q = %d"
        self.attention_text = "A = %d"
        self.meditation_text = "M = %d"
        self.quality_label = Label(master = self, width = self.object_width + self.label_dw,
                                   height = self.object_height, text = self.quality_text,
                                   bg = '#FFFF77')
        self.attention_label = Label(master = self, width = self.object_width + self.label_dw,
                                   height = self.object_height, text = self.attention_text,
                                   bg = '#FFCC55')
        self.meditation_label = Label(master = self, width = self.object_width + self.label_dw,
                                   height = self.object_height, text = self.meditation_text,
                                   bg = '#22FFFF')
        self.quality_label.grid(row = 1, column = 0)
        self.attention_label.grid(row = 2, column = 0)
        self.meditation_label.grid(row = 3, column = 0)

class SignalViewer(Tk):

    def __init__(self):
        Tk.__init__(self)
        self.frame_height = [800, 800]
        self.frame_width = [200, 1500]
        # Frames
        self.signal_frame = drawer.Drawer(tk = self,
                                          grid_position = None,#(0, 1),
                                          height = self.frame_height[1],
                                          width = self.frame_width[1])
        self.value_frame = ControlFrame(tk = self,
                                 height = self.frame_height[0],
                                 width = self.frame_width[0],
                                 bg = '#EEFFEE')
        self.value_frame.grid(row = 0, column = 0)
        self.signal_frame.grid(row = 0, column = 1)
        # UDP
        self.udp_port = 9999
        self.udp_server = UDPServer(self.udp_port)
        self.n_signal_samples = 2000
        self.signal_limits = [-2**11, 2**11]
        self.spectrum_limits = [0, 10000]
        self.signal_contraction = 1.1
        self.signal_buffer = deque([0] * self.n_signal_samples, maxlen = self.n_signal_samples)
        self.spectrum_buffer = deque([0] * self.n_signal_samples, maxlen = self.n_signal_samples)
        self.saved_signal = []
        self.n_saved = 10000
        self.saved_file = open('saved.txt', 'w')
        # Setup
        self.setup()

    def flush_saved(self):
        for s in self.saved_signal:
            self.saved_file.write('%d ' % s)
        self.saved_file.close()
        self.stop()

    def udp_update(self):
        self.udp_server.read_msg()
        for s in self.udp_server.msg.take_signal():
            if s < self.signal_limits[0] : s = self.signal_limits[0]
            if s > self.signal_limits[1] : s = self.signal_limits[1]
            self.signal_buffer.append(s)


            
            if not self.udp_server.msg.signal_quality:
                self.saved_signal.append(s)
                if len(self.saved_signal) >= self.n_saved:
                    self.flush_saved()



        
        self.signal_frame.curves['raw'].update(pointsY = np.array(self.signal_buffer))
        #self.signal_frame.curves['fft'].update(pointsY = np.fft.rfft(np.array(self.signal_buffer)))
        # labels
        self.value_frame.quality_label["text"] = (
            self.value_frame.quality_text % self.udp_server.msg.signal_quality)
        self.value_frame.attention_label["text"] = (
            self.value_frame.attention_text % self.udp_server.msg.attention)
        self.value_frame.meditation_label["text"] = (
            self.value_frame.meditation_text % self.udp_server.msg.meditation)
        #print(self.udp_server.msg.time_points)
        self.update()

    def setup(self):
        self.n_curves = 1
        self.signal_frame.add_canvas('Signal',
                                     height = self.signal_frame['height'] / self.n_curves,
                                     width = self.signal_frame['width'])
        self.signal_frame.add_task('update', 'Signal',
                                   func = self.udp_update, period_ms = 0)
        self.signal_frame.add_curve('raw', 'Signal',
                                    pointsX = np.array(list(range(self.n_signal_samples))),
                                    pointsY = np.array(self.signal_buffer, float))
        self.signal_frame.curves['raw'].scaleX = [0, self.n_signal_samples]
        self.signal_frame.curves['raw'].scaleY = (np.array(self.signal_limits) *
                                                  self.signal_contraction)
##        self.signal_frame.curves['raw'].update(
##            pointsX = np.array(list(range(self.n_signal_samples))),
##            pointsY = np.array(self.signal_buffer, float))
        # Spectrum
##        self.signal_frame.add_canvas('Spectrum',
##                                     height = self.signal_frame['height'] / self.n_curves,
##                                     width = self.signal_frame['width'], grid_position = [1,0])
##        self.signal_frame.add_curve('fft', 'Spectrum',
##                                    pointsX = np.array(list(range(self.n_signal_samples))),
##                                    pointsY = np.array(self.spectrum_buffer, float))
##        self.signal_frame.curves['fft'].scaleX = [0, self.n_signal_samples]
##        self.signal_frame.curves['fft'].scaleY = (np.array(self.spectrum_limits) *
##                                                  self.signal_contraction)


    def start(self):
        self.signal_frame.tasks['update'].start()

    def stop(self):
        self.signal_frame.tasks['update'].stop()

if __name__ == "__main__":
    sigview = SignalViewer()
    sigview.update()
    





















    
    
                                    
