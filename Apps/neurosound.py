from importlib import reload  
import neurorec
reload(neurorec)
import wavplayer
import sys
#PyToolsPath = "/home/shisius/Projects/PyTools"
PyToolsPath = "../../radarlib/scripts"
if not(PyToolsPath in sys.path):
    sys.path.append(PyToolsPath)
import drawer
import numpy as np
from tkinter import *
import time
import json



class SoundEEG_Control(Frame):

    def __init__(self, tk = None, **kwargs):
        Frame.__init__(self, master = tk, **kwargs)
        self.tk = tk
        self.font_size = 10
        self.fields = {}
        self.pixel = PhotoImage(width=1, height=1)
        self.grid_propagate(False)
        self.columnconfigure(0, weight = 10)

    def update_fields(self):
        
        for f in self.fields.values():
            f['width'] = self['width']
            f['height'] = self['height'] // len(self.fields.values())
            f['font'] = ('', f['width'] // 10)
        self.update()

    def add_field(self, field_type, field_name, **kwargs):
        if field_type == 'button':
            self.fields[field_name] = Button(master = self, image=self.pixel, compound = 'c', **kwargs)
            self.fields[field_name]['text'] = field_name
        elif field_type == 'text':
            self.fields[field_name] = Text(master = self, **kwargs)
        else:
            self.fields[field_name] = Label(master = self, image=self.pixel, compound = 'c', **kwargs)
        self.fields[field_name].grid(row = len(self.fields) - 1, column = 0)
        self.update_fields()

class SoundEEG_GUI(Tk):

    DEFAULT_CTL_FIELDS = {'connect': {'type': 'button',
                                      'command': None,
                                      'color': '#007700'},
                          'connect_response': {'type': 'label',
                                               'command': None,
                                               'color': '#FFFFFF'},
                          'choose_wav': {'type': 'button',
                                         'command': None,
                                         'color': '#8888FF'},
                          'current_wav': {'type': 'label',
                                          'command': None,
                                          'color': '#FFFFFF'},
                          'play+EEG': {'type': 'button',
                                       'command': None,
                                       'color': '#009999'}}

    def __init__(self, height = 400, width = 1000):
        Tk.__init__(self)
        self.title("SoundEEG")
        self.height = height
        self.width = width
        self.width_ratio = 7
        self.signal_frame = drawer.Drawer(tk = self,
                                          grid_position = None,
                                          height = self.height,
                                          width = self.width * self.width_ratio // (self.width_ratio + 1))
        self.signal_frame.grid(row = 0, column = 0)
        self.control_frame = SoundEEG_Control(tk = self)
        self.control_frame.grid(row = 0, column = 1)
        self.control_fields = dict(self.DEFAULT_CTL_FIELDS)
        self.control_fields['connect']['command'] = self.connect
        self.control_fields['choose_wav']['command'] = self.choose_wav
        self.control_fields['play+EEG']['command'] = self.playEEG
        # Objects
        self.wav_player = wavplayer.WavPlayer()
        self.bci_comm = neurorec.GanglionControl()

    def setup(self):
        self.setup_control_frame()
        self.setup_signal_frame()

    def update_curves(self):
        for ch in self.bci_comm.channels_position.keys():
            self.signal_frame.curves[self.bci_comm.channels_position[ch]].update(pointsY = np.array(self.bci_comm.eeg_buffer[ch]))

    def update_frames(self):
        self.control_frame['height'] = self.height
        self.control_frame['width'] = self.width // (self.width_ratio + 1)
        #self.control_frame.update_fields()
        for c in self.signal_frame.canvases.values():
            c['height'] = self.height // len(self.signal_frame.canvases.values())
            c['width'] = self.width * self.width_ratio // (self.width_ratio + 1)
        self.width = self.control_frame['width'] + self.signal_frame['width']

    def update(self):
        if self.bci_comm.reading:
            self.bci_comm.find_read_raw33()
            self.bci_comm.parse_raw33()
        #self.height = self['height']#self.winfo_height() - 14
        #self.width = self['width']#self.winfo_width() - 14
        #self.update_frames()
        if self.bci_comm.expected_sample_index == 0:
            self.update_curves()
            Tk.update(self)
        if self.wav_player.is_playing:
            if (not self.wav_player.get_is_playing()) and self.bci_comm.recording :
                self.stopEEG()

    def setup_control_frame(self):
        for f in self.control_fields:
            self.control_frame.add_field(self.control_fields[f]['type'],
                                         f, bg=self.control_fields[f]['color'],
                                         command=self.control_fields[f]['command'])
        

    def setup_signal_frame(self):
        for ch in self.bci_comm.channels_position.keys():
            self.signal_frame.add_canvas(self.bci_comm.channels_position[ch], grid_position = [ch,0])
            self.signal_frame.add_curve(self.bci_comm.channels_position[ch], self.bci_comm.channels_position[ch],
                                        pointsX = np.array(list(range(self.bci_comm.eeg_buffer_depth))),
                                        pointsY = np.array(self.bci_comm.eeg_buffer[ch]))
            self.signal_frame.curves[self.bci_comm.channels_position[ch]].scaleX = [0, self.bci_comm.eeg_buffer_depth]
            self.signal_frame.curves[self.bci_comm.channels_position[ch]].scaleY = [-2**(self.bci_comm.RESOLUTION_BITS-1),
                                                                                    2**(self.bci_comm.RESOLUTION_BITS-1)]
        self.signal_frame.add_task('update', self.bci_comm.channels_position[0],
                                   func = self.update, period_ms = 0)
        self.signal_frame.tasks['update'].start()

    def set_scale(self, new_scale_x, new_scale_y):
        for ch in self.bci_comm.channels_position.keys():
            self.signal_frame.curves[self.bci_comm.channels_position[ch]].scaleX = [0, new_scale_x]
            self.signal_frame.curves[self.bci_comm.channels_position[ch]].scaleY = [-new_scale_y, new_scale_y]

    def connect(self):
        if not self.bci_comm.connected:
            self.bci_comm.setup_tcp()
            self.control_frame.fields['connection_response'] = self.bci_comm.http_response
            if self.bci_comm.connected:
                self.control_frame.fields['connect']['text'] = 'disconnect'
            else:
                self.control_frame.fields['connect']['text'] = self.bci_comm.error
                return
        else:
            self.bci_comm.stop_tcp()
            self.bci_comm.close_tcp()
            self.control_frame.fields['connect']['text'] = 'connect'
            return
        self.bci_comm.start_tcp()
        print('Start reading process')
        #self.bci_comm.reading_process_tcp()

    def choose_wav(self):
        self.wav_player.choose_wav()
        self.control_frame.fields['current_wav']['text'] = self.wav_player.session_name

    def stopEEG(self):
        self.bci_comm.recording = False
        fd = open('sample.eeg', 'wb')
        self.bci_comm.eeg_record['sample_rate'] = self.bci_comm.sample_rate
        fd.write(bytes(json.dumps(self.bci_comm.eeg_record), encoding = 'UTF-8'))
        self.bci_comm.eeg_record = dict(bci_comm.SOUND_EEG_DATA)
        self.control_frame.fields['play+EEG']['text'] = 'play+EEG'

    def playEEG(self):
        if not self.bci_comm.recording:
            if self.wav_player.wave_obj != None:
                self.wav_player.play()
            self.bci_comm.recording = True
            self.control_frame.fields['play+EEG']['text'] = 'stop'
        else:
            self.bci_comm.recording = False
            if self.wav_player.is_playing:
                if self.wav_player.get_is_playing():
                    self.wav_player.stop()
            self.stopEEG()
            


if __name__ == "__main__":
    neurorec.DEFAULT_EEG_BUFFER_DEPTH = 1500
    gui = SoundEEG_GUI(height = 1000, width = 1900)
    gui.bci_comm.sample_rate = 200
    gui.setup()
    print(gui.bci_comm.http_response)
    gui.set_scale(500, 1000000)
    gui.update_frames()
    gui.update()
    
