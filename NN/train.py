from __future__ import print_function
import argparse
import torch

import torch.nn.functional as F
import torch.optim as optim
from torchvision import transforms
from torch.optim.lr_scheduler import StepLR, ExponentialLR
from torch.utils.data import IterableDataset
from tqdm import tqdm
from random import shuffle, uniform
import numpy as np
import torch
import os

from sklearn.metrics import roc_curve
from scipy.optimize import brentq
from scipy.interpolate import interp1d

import matplotlib.pyplot as plt

from datagen import Dataset
from models.unet.unet_model import UNet
PATH_TO_DATA = 'D:\\Datasets\\EEG_MUSIC\\Naturalistic Music EEG Dataset - Tempo (NMED-T)\\ready'

SAMPLE_RATE = 16000
WIN_LEN = 16000
NSTFT = 1024
HOP_STFT = 128


def plot_images(image, t_bound, p_bound, name):
    # Create figure
    num = 0
    tar_bound, pred_bound = t_bound[num].cpu().detach().numpy(), p_bound[num].cpu().detach().numpy()

    im = image[num].permute(1, 2, 0).cpu().detach().numpy()
    fig, (ax1, ax2) = plt.subplots(1, 2)
    im = (im - im.min()) / (im.max() - im.min())
    im_1 = im
    im[:, :, 0] = tar_bound / tar_bound.max()
    ax1.imshow(im)
    ax1.set_title(f"Should be {np.max(tar_bound).max()}")
    max_layer = np.argmax(pred_bound, 0).max()
    pred_layer = torch.softmax(torch.tensor(pred_bound), dim=0)[max_layer].numpy()
    im_1[:, :, 0] = pred_layer / pred_layer.max()
    ax2.imshow(im_1)
    ax2.set_title(f"We have {max_layer}")
    plt.savefig(name)


def train(log_interval, model, device, train_loader, optimizer, epoch):
    model.train()
    for batch_idx, (data, target) in tqdm(enumerate(train_loader)):
        data, target = data.to(device), target.to(device)
        optimizer.zero_grad()
        output = model(data)

        loss = F.mse_loss(output, target)
        loss.backward()
        max_grad = torch.nn.utils.clip_grad_norm(model.parameters(), max_norm=3)
        optimizer.step()
        if batch_idx % log_interval == 0:
            print('Train Epoch: {} [{}/{} ({:.0f}%)]\tLoss: {:.6f}\tGrad norm: {:.2f}'.format(
                epoch, batch_idx * len(data), len(train_loader.dataset),
                       100. * batch_idx / len(train_loader), loss.item(), max_grad.item()))
            plt.imshow(output[0].detach().cpu().numpy())
            plt.savefig('train_spec_predicted.png')
            plt.close()
            plt.imshow(target[0].detach().cpu().numpy())
            plt.savefig('train_spec_target.png')
            plt.close()


def test(model, device, test_loader):
    model.eval()
    test_loss = 0
    y, y_score = [], []
    with torch.no_grad():
        for data, target in test_loader:
            data, target = data.to(device), target.to(device)
            output = model(data)
            test_loss += F.mse_loss(output, target).item()  # sum up batch loss
            y.append(target)

    test_loss /= len(test_loader.dataset)
    # plot_images(data, target, output, 'im.png')
    print('\nTest set: Average loss: {:.4f},\n'.format(test_loss))
    plt.imshow(output[0].detach().cpu().numpy())
    plt.savefig('test_spec_predicted.png')
    plt.close()
    plt.imshow(target[0].detach().cpu().numpy())
    plt.savefig('test_spec_target.png')
    plt.close()


def main():
    # Training settings

    use_cuda = torch.cuda.is_available()

    torch.manual_seed(1234)

    device = torch.device("cuda" if use_cuda else "cpu")

    train_kwargs = {'batch_size': 16}
    test_kwargs = {'batch_size': 16}
    if use_cuda:
        cuda_kwargs = {'num_workers': 1,
                       'pin_memory': True,
                       'shuffle': True}
        train_kwargs.update(cuda_kwargs)
        test_kwargs.update(cuda_kwargs)

    batch_size = 32
    lr = 5e-3
    gamma = 0.95
    epochs = 100
    log_interval = 20

    dataset1 = Dataset(PATH_TO_DATA, part_to_train=0.8)
    dataset2 = Dataset(PATH_TO_DATA, is_train=False, part_to_train=0.9)
    train_loader = torch.utils.data.DataLoader(dataset1, batch_size=batch_size, num_workers=1)
    test_loader = torch.utils.data.DataLoader(dataset2, batch_size=batch_size, num_workers=1)

    model = UNet(125, 257).to(device)

    optimizer = optim.Adam(model.parameters(), lr=lr)

    scheduler = ExponentialLR(optimizer, gamma=gamma)
    for epoch in range(1, epochs + 1):
        print('Training...')
        train(log_interval, model, device, train_loader, optimizer, epoch)
        print('Testing...')
        test(model, device, test_loader)
        scheduler.step()
        torch.save(model.state_dict(), "eeg_msc.pt")

    # if args.save_model:
    #     torch.save(model.state_dict(), "drone_finder.pt")


if __name__ == '__main__':
    main()
