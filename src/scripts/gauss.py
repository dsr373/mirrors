#!/usr/bin/python3

import os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib

from util import *

CONFIG = "config/gauss_hm.txt"
SAVE_DIR = os.path.join("fig", "gauss")

FONTSIZE = 16
CAPSIZE = 7
FIT_LABEL = "fit: $y={:4.4f}x{:+4.4f}$"
FIGSIZE = (9, 8)

def plot_sizes(data):
    rs = data[1]            # radii
    sig = data[2]           # taper
    mins = data[3]          # minima location
    sig_mins = data[4]      # error

    xs = np.divide(sig, rs)
    ys = np.array(mins)
    dys = np.array(sig_mins)

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # plot the data
    fig, ax = plt.subplots(figsize=FIGSIZE)
    ax.errorbar(xs, ys, yerr=dys, fmt='+', label="data", markersize=FONTSIZE)
    # ax.set_yscale("log")
    # ax.set_xscale("log")

    # fit a line
    # coefs = np.polyfit(xs, ys, 1, w=1/dys)
    # print("Airy disc fit coefficients, largest power first: ", coefs)
    # fit_label = FIT_LABEL.format(coefs[0], coefs[1])
    # ax.plot(xs, np.polyval(coefs, xs), '-', label=fit_label)

    # label the plot
    ax.set_xlabel("$\\sigma / R$")
    ax.set_ylabel("central spot radius (m)")
    ax.set_title("Gaussian illumination spot size")
    ax.legend()

    plt.savefig(os.path.join(SAVE_DIR, "size.pdf"))


def plot_intensities(data):
    rs = data[1]                        # radii
    sig = data[2]                       # taper
    rel_amp = data[5]/np.min(data[5])   # relative amplitude
    ys = np.power(rel_amp, 2)           # relative intensity

    xs = np.divide(sig, rs)

    # formatting
    matplotlib.rcParams.update({'font.size': FONTSIZE})

    # plot
    fig, ax = plt.subplots(figsize=FIGSIZE)
    ax.plot(xs, ys, '+', label="data", markersize=FONTSIZE)

    # label the plot
    ax.set_xlabel("$\\sigma / R$")
    ax.set_ylabel("central amplitude (arbitrary)")
    ax.set_title("Gaussian illumination central intensity")
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,3), useMathText=True)
    # ax.legend()

    plt.savefig(os.path.join(SAVE_DIR, "int.pdf"))


if __name__ == "__main__":
    print_warning([CONFIG])

    os.makedirs(SAVE_DIR, exist_ok=True)

    data = read_data(CONFIG)
    plot_sizes(data)
    plot_intensities(data)
    plt.show()