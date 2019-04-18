#!/usr/bin/python3

import os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib

from util import *

FONTSIZE = 16
CAPSIZE = 7
FIT_LABEL = "$y={:4.4f}x{:+4.4f}$"
FIGSIZE = (9, 8)

def accumulate_data(data, xidx, yidx, dyidx=None):
    """
    Accumulate the data points with the same x value into the same data point
    with error given by the standard deviation.
    xidx, yidx, dyidx are the indices of the columns to use as x, y and error respectively
    """
    xs = data[xidx]
    ys = data[yidx]
    if dyidx:
        dys = data[dyidx]
    else:
        dys = np.zeros(len(ys))
    res = ([], [], [])

    tmp = []
    x_current = xs[0]

    for i, x in enumerate(xs):
        if x == x_current:
            tmp.append(ys[i])
        else:
            # if the current x has changed, push the values to res and start a new run
            res[0].append(x_current)
            res[1].append(np.average(tmp))
            res[2].append(np.maximum(np.std(tmp), dys[i]))
            print(tmp)

            x_current = x
            tmp = [ys[i]]
    
    return res

def plot_sizes(data):
    xs = data[0]          # phase error std dev
    ys = data[1]          # fwhp
    dys = data[2]         # error

    # xs = np.array(sigerr)
    # ys = np.array(ps)
    # dys = np.array(sig_ps)

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # plot the data
    fig, ax = plt.subplots(figsize=FIGSIZE)
    ax.errorbar(xs, ys, yerr=dys, fmt='+', label="data", markersize=FONTSIZE)

    # label the plot
    ax.set_xlabel("$\\sigma_{{\\epsilon}}$")
    ax.set_ylabel("Full Width at Half Power $(m^{{-1}})$")
    ax.set_title("Central width with phase errors")
    ax.legend()

    # plt.savefig(os.path.join(SAVE_DIR, "size.pdf"))


def plot_intensities(data):
    sigerr = data[0]                    # phase error std dev
    amp = data[1]                       # amplitude
    sig_amp = data[2]                   # std dev in amplitudes

    xs = np.power(np.array(sigerr), 2)
    ys = np.array(amp / np.min(amp))
    dys = np.array(sig_amp / np.min(amp))

    # formatting
    matplotlib.rcParams.update({'font.size': FONTSIZE})

    # plot
    fig, ax = plt.subplots(figsize=FIGSIZE)
    ax.errorbar(xs, ys, yerr=dys, fmt='+', label="data", markersize=FONTSIZE)
    ax.set_yscale("log")

    # label the plot
    ax.set_xlabel("$\\sigma_{{\\epsilon}}$")
    ax.set_ylabel("central intensity (arbitrary)")
    ax.set_title("Central intensity with phase errors")
    # ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,3), useMathText=True)
    # ax.legend()

    # plt.savefig(os.path.join(SAVE_DIR, "int.pdf"))


CONFIG1 = "config/rand.txt"
# CONFIG2 = "config/gauss_amp.txt"
SAVE_DIR = os.path.join("fig", "rand")

if __name__ == "__main__":
    print_warning([CONFIG1])

    os.makedirs(SAVE_DIR, exist_ok=True)

    data = read_data(CONFIG1)
    plot_sizes(accumulate_data(data, 4, 6, 7))
    plot_intensities(accumulate_data(data, 4, 8, None))
    plt.show()