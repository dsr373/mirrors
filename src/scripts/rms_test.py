#!/usr/bin/python3

import os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib

from util import *

FONTSIZE = 18
CAPSIZE = 7
FIT_LABEL = "$y={:4.4f}x{:+4.4f}$"
FIT_INFO = "Slope: {:4.6f} +/- {:4.6f}\nIntercept: {:4.6f} +/- {:4.6f}"
FIGSIZE = (9, 8)


def plot_depth(data):
    xs = np.array(data[0])    # desired phase error
    ys = np.array(data[1])    # obtained phase error
    dys = np.array(data[2])   # error

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # plot the data
    fig, ax = plt.subplots(figsize=FIGSIZE)
    ax.errorbar(xs, ys, yerr=dys, fmt='+', label="data", markersize=FONTSIZE)

    # fit a line
    # first, find the xs between 0 and pi/2
    idx = [i for i, x in enumerate(xs) if x >= 0 and x <= np.pi/2]
    
    coefs, covar = np.polyfit(xs[idx], ys[idx], 1, cov=True)
    print(FIT_INFO.format(coefs[0], np.sqrt(covar[0, 0]), coefs[1], np.sqrt(covar[1, 1])))

    fit_label = "Line fit: " + FIT_LABEL.format(coefs[0], coefs[1])
    ax.plot(xs[idx], np.polyval(coefs, xs[idx]), '-', label=fit_label)

    # label the plot
    ax.set_xlabel("Desired phase RMS (radians)")
    ax.set_ylabel("Resulting phase RMS (radians)")
    ax.legend()

    plt.savefig(os.path.join(SAVE_DIR, "depth.pdf"), bbox_inches="tight")


def plot_lc(data):
    xs = data[0]    # correlation length
    ys = data[1]    # obtained phase error
    dys = data[2]   # error

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # plot the data
    fig, ax = plt.subplots(figsize=FIGSIZE)
    ax.errorbar(xs, ys, yerr=dys, fmt='+', label="data", markersize=FONTSIZE)

    # label the plot
    ax.set_xlabel("Correlation length (m)")
    ax.set_ylabel("Resulting phase RMS (radians)")
    # ax.legend()

    plt.savefig(os.path.join(SAVE_DIR, "l_corr.pdf"), bbox_inches="tight")


CONFIG1 = "config/rms_test_depth.txt"
CONFIG2 = "config/rms_test_lc.txt"
SAVE_DIR = os.path.join("fig", "rmstest")

if __name__ == "__main__":
    print_warning([CONFIG1, CONFIG2])

    os.makedirs(SAVE_DIR, exist_ok=True)

    data = read_data(CONFIG1)
    plot_depth(accumulate_data(data, 4, 8, None))
    
    data = read_data(CONFIG2)
    plot_lc(accumulate_data(data, 6, 8, None))
    plt.show()