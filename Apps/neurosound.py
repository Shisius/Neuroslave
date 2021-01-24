import neurorec
import wavplayer
import sys
PyToolsPath = "/home/shisius/Projects/PyTools"
if not(PyToolsPath in sys.path):
    sys.path.append(PyToolsPath)
import drawer
import numpy as np
from tkinter import *
import time



class SoundEEG_Control(Frame):

    def __init__(self, tk = None, **kwargs):
        Frame.__init__(self, master = tk, **kwargs)
        self.tk = tk
        self.font_size = 10
        self.fields = {}
        self.pixel = PhotoImage(width=1, height=1)

    def update_fields(self):
        for f in self.fields.values():
            f['width'] = self['width']
            f['height'] = self['height'] // len(self.fields.values())
            f['font'] = self.font_size
        self.update()

    def add_field(self, field_type, field_name, **kwargs):
        if field_type == 'button':
            self.fields[field_name] = Button(master = self, image=self.pixel, compound = 'c', **kwargs)
            self.fields[field_name]['text'] = field_name
        else:
            self.fields[field_name] = Label(master = self, image=self.pixel, compound = 'c', **kwargs)
        self.fields[field_name].grid(row = len(self.fields) - 1, column = 0)
        self.update_fields()

class SoundEEG_GUI(Tk):

    DEFAULT_CTL_FIELDS = {'connect': {'type': 'button',
                                      'func': None,
                                      'color': '#007700'}}

    def __init__(self, height = 400, width = 1000):
        Tk.__init__(self)
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
        self.control_frame.update_fields()
        for c in self.signal_frame.canvases.values():
            c['height'] = self.height // len(self.signal_frame.canvases.values())
            c['width'] = self.width * self.width_ratio // (self.width_ratio + 1)
        self.width = self.control_frame['width'] + self.signal_frame['width']

    def update(self):
        self.height = self.winfo_height() - 14
        self.width = self.winfo_width() - 14
        self.update_frames()
        self.update_curves()
        Tk.update(self)

    def setup_control_frame(self):
        for f in self.control_fields:
            self.control_frame.add_field(self.control_fields[f]['type'],
                                         f, bg=self.control_fields[f]['color'])

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
        #self.signal_frame.tasks['update'].start()



if __name__ == "__main__":
    gui = SoundEEG_GUI()
    gui.setup()
    gui.update()
