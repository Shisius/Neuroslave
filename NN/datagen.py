import torch
from utils import z_norm, chunker, stft, stft_eeg, add_rand_noise, to_log
import os
import scipy.io
import librosa
from pydub import AudioSegment
import numpy as np


class Dataset(torch.utils.data.Dataset):
    """Characterizes a dataset for PyTorch"""

    def __init__(self, path_to_data, is_train=True, part_to_train=0.1, sr=8000, sample_len=1, use_noise=False):
        """Initialization"""
        path_to_eeg = path_to_data + '/eeg'
        path_to_music = path_to_data + '/music'
        self.sample_len = int(sample_len * sr)
        self.eeg = []
        self.music = []
        self.sr = sr
        self.use_noise = use_noise
        filelist_music = []
        for root, sub, files in os.walk(path_to_music):
            filelist_music.append(sorted(files))

        filelist_eeg = []
        for root, sub, files in os.walk(path_to_eeg):
            filelist_eeg.append(sorted(files))
        full_len = len(filelist_music[0])
        if is_train:
            filelist_music = filelist_music[0][:int(full_len * part_to_train)]
            filelist_eeg = filelist_eeg[0][:int(full_len * part_to_train)]
        else:
            filelist_music = filelist_music[0][int(full_len * part_to_train):]
            filelist_eeg = filelist_eeg[0][int(full_len * part_to_train):]

        for file in filelist_music:
            # audio = AudioSegment.from_file(path_to_music + '/' + file, **{'channels': 1})
            # arr = audio.get_array_of_samples()
            # np_arr = np.array(arr)
            # np_arr = np_arr / np_arr.max()
            np_arr, _ = librosa.core.load(path_to_music + '/' + file, sr=sr)
            self.music.append(np_arr)

        for file in filelist_eeg:
            mat = scipy.io.loadmat(path_to_eeg + '/' + file)
            self.eeg.append(mat['data'+file.split('song')[1].split('_')[0]])
            self.eeg_sr = mat['fs'][0][0]

        self.list_IDs = np.arange(len(self.eeg) * 20 * len(np_arr) // 8000)

    def __len__(self):
        """Denotes the total number of samples"""
        return len(self.list_IDs)

    def __getitem__(self, index, mus_win=512, mus_hop=256, eeg_win=32, eeg_hop=2):
        """Generates one sample of data"""
        # Select sample
        index_rand = round(np.random.uniform(0, len(self.eeg) - 1))
        X = chunker(self.eeg[index_rand], self.music[index_rand],  self.sample_len, self.eeg_sr, self.sr)
        X_m = stft(X[1], self.sr, mus_win, mus_hop)
        X_e = stft_eeg(X[0], self.sr, eeg_win, eeg_hop)
        if self.use_noise:
            X_m = add_rand_noise(abs(X_m))
            X_e = add_rand_noise(abs(X_e))
        X_m = to_log(abs(X_m) + 1e-6)
        X_e = z_norm(X_e)
        X_e = to_log(abs(X_e) + 1e-6)
        X_e = z_norm(X_e)
        X_e = torch.tensor(X_e).float()
        X_m = torch.tensor(X_m).float()
        X_m = (X_m - X_m.mean(dim=0, keepdim=True)) / (X_m.std(dim=0, keepdim=True) + 1e-6)
        for i in np.arange(X_e.size(0)):
            X_e[i] = (X_e[i] - X_e[i].mean(dim=0, keepdim=True)) / (X_e[i].std(dim=0, keepdim=True) + 1e-6)
        return X_e, X_m