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
X_LABEL = "$\\sigma_{{\\epsilon}}/\\lambda$"
FIGSIZE = (9, 8)

def plot_sizes(data):
    xs = np.divide(data[0], 4 * np.pi)    # phase error std dev
    ys = data[1]          # fwhp
    dys = data[2]         # error

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # plot the data
    fig, ax = plt.subplots(figsize=FIGSIZE)
    ax.errorbar(xs, ys, yerr=dys, fmt='+', label="data", markersize=FONTSIZE)

    # label the plot
    ax.set_xlabel(X_LABEL)
    ax.set_ylabel("Full Width at Half Power $(m^{{-1}})$")
    ax.set_title("Central width with phase errors")
    # ax.legend()

    plt.savefig(os.path.join(SAVE_DIR, "size.pdf"), bbox_inches="tight")


def plot_intensities(data):
    sigerr = np.divide(data[0], 4 * np.pi)  # phase error std dev
    amp = data[1] / np.min(data[1])         # amplitude normalised
    sig_amp = data[2] / np.min(data[1])     # error in amplitudes normalised

    # plot log of amplitude vs square of phase deviation
    xs = np.power(np.array(sigerr), 2)
    ys = np.log(amp)
    dys = np.divide(sig_amp, amp)

    # formatting
    matplotlib.rcParams.update({'font.size': FONTSIZE})

    # plot
    fig, ax = plt.subplots(figsize=FIGSIZE)
    ax.errorbar(xs, ys, yerr=dys, fmt='+', label="data", markersize=FONTSIZE)

    # fit a line
    coefs, covar = np.polyfit(xs, ys, 1, w=1/dys, cov=True)
    fit_label = "Line fit: " + FIT_LABEL.format(coefs[0], coefs[1])
    print(FIT_INFO.format(coefs[0], np.sqrt(covar[0, 0]), coefs[1], np.sqrt(covar[1, 1])))
    ax.plot(xs, np.polyval(coefs, xs), '-', label=fit_label)

    # label the plot
    ax.set_xlabel("({:s})$^2$".format(X_LABEL))
    ax.set_ylabel("$\\ln(\\psi_0\\ /\\ arb.\\ units)$")
    ax.set_title("Central amplitude vs phase errors")
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-2,3), useMathText=True)
    ax.legend()

    plt.savefig(os.path.join(SAVE_DIR, "int.pdf"), bbox_inches="tight")


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