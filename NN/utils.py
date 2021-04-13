from scipy import signal
import librosa
from random import random
import numpy as np
from random import uniform


def wav_load(path, sr):
    wave, _ = librosa.load(path, sr=sr)
    return wave


def chunker(eeg, wave, length, eeg_sr, sr):
    rand_brain = int(np.random.uniform(0, 20))
    ratio = 128
    if len(wave) > length:
        rand_int = random()
        rand_msc = int((len(wave) - length * 3) * rand_int)
        rand_eeg = int((np.shape(eeg)[1] - length / ratio * 3) * rand_int)
        wave = wave[rand_msc:rand_msc + length]
        eeg = eeg[:, rand_eeg: 1 + rand_eeg + length // ratio, rand_brain]
    else:
        wave = np.pad(wave, (0, (length - len(wave))), mode='symmetric')
        eeg = np.pad(eeg, (0, (length * eeg_sr / sr - len(eeg))), mode='symmetric')
    return eeg, wave


def stft(wave, sr, n_stft, hop):
    _, _, Zxx = signal.stft(wave, sr, window='hann', nperseg=n_stft, noverlap=n_stft-hop)
    return Zxx


def stft_eeg(eeg, sr, n_stft, hop):
    eeg_lst = []
    for i in np.arange(np.shape(eeg)[0]):
        _, _, Zxx = signal.stft(eeg[i], sr, window='hann', nperseg=n_stft, noverlap=n_stft - hop)
        eeg_lst.append(Zxx)
    eeg = np.array(eeg_lst)
    return eeg


def to_log(stft):
    stft = np.log(np.abs(stft) + 1e-7)
    return stft


def z_norm(X):
    X = (X - np.mean(X)) / (np.std(X) + 1e-7)
    return X


def add_rand_noise(x):

    clean_db = 10 * np.log(np.mean(x) ** 2 + 1e-4)
    noise_snr = uniform(0, 30)

    noise = np.random.rand(np.shape(x)[0], np.shape(x)[1])
    noise_db = 10 * np.log(np.mean(noise) ** 2 + 1e-4)
    noise_mag = np.sqrt(10 ** ((clean_db - noise_db - noise_snr) / 10))

    return x + noise_mag * noise