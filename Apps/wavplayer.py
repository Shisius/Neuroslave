import simpleaudio as sa
#import easygui as egui
import os
import time
import struct

WAV_EXT = '.wav'

class WavPlayer:

    UNPACK_PULES = {1 : 'b',
                    2 : 'h',
                    4 : 'i'}

    def __init__(self):
        # File
        self.wav_file = None
        self.wav_file_dir = ''
        self.session_name = ''
        # Wav objects
        self.wave_obj = None
        self.play_obj = None
        self.n_samples = 0
        self.track_array = []
        # Current track
        self.is_playing = False
        self.current_time = time.time()
        self.start_time = time.time()
        self.current_sample = 0
        self.track_buffer = []
        self.track_buffer_length = 1024

    def play(self):
        self.play_obj = self.wave_obj.play()
        self.start_time = time.time()
        self.is_playing = self.play_obj.is_playing()

    def stop(self):
        self.play_obj.stop()
        self.is_playing = self.play_obj.is_playing()

    def get_is_playing(self):
        self.is_playing = self.play_obj.is_playing()
        return self.is_playing

    def update(self):
        self.is_playing = self.play_obj.is_playing()
        self.current_time = time.time()
        self.current_sample = round((self.current_time - self.start_time) * self.wave_obj.sample_rate)
        #self.track_buffer = struct.unpack(str(self.track_buffer_length))

    def choose_wav(self, file_path = ''):
        if file_path == '':
            #self.wav_file = egui.fileopenbox()
            #self.wav_file_dir = os.path.dirname(self.wav_file)
            return
        else:
            self.wav_file = file_path
            self.wav_file_dir = file_path
        self.wave_obj = sa.WaveObject.from_wave_file(self.wav_file)
        self.session_name = self.wav_file[len(self.wav_file_dir)+1:self.wav_file.rfind(WAV_EXT)]
        self.track_buffer = [0] * self.track_buffer_length
        self.n_samples = len(self.wave_obj.audio_data) // (self.wave_obj.num_channels *
                                                           self.wave_obj.bytes_per_sample)
        self.track_array = struct.unpack(str(self.n_samples * self.wave_obj.num_channels)
                                         + self.UNPACK_PULES[self.wave_obj.bytes_per_sample],
                                         self.wave_obj.audio_data)

if __name__ == "__main__":
    w = WavPlayer()
    w.choose_wav('wav.wav')
    w.play()
    #w.play_obj.wait_done()
    while w.get_is_playing():
        w.update()
        print(w.current_sample, w.n_samples)

    
