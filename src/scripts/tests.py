#!/usr/bin/python3

import re
import os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib

from util import *

SIZES_CONFIG = "config/circular_mins.txt"
AMP_CONFIG = "config/circular_amps.txt"
RECT_CONFIG = "config/rectangle_mins.txt"
SAVE_DIR = "fig/tests"

FONTSIZE = 16
CAPSIZE = 7
FIT_LABEL = "fit: $y={:4.4f}x{:+4.4f}$"
FIGSIZE = (9, 8)

SEP = "="*30

def plot_sizes(data):
    xs = 1/data[1]      # inverse aperture diameter
    ys = data[2]        # image size
    dys = data[3]       # image size error

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # plot the data
    fig, ax = plt.subplots(figsize=FIGSIZE)
    ax.errorbar(xs, ys, yerr=dys, fmt='+', label="data", markersize=FONTSIZE)

    # fit a line
    coefs = np.polyfit(xs, ys, 1, w=1/dys)
    print("Airy disc fit coefficients, largest power first: ", coefs)
    fit_label = FIT_LABEL.format(coefs[0], coefs[1])
    ax.plot(xs, np.polyval(coefs, xs), '-', label=fit_label)

    # label the plot
    ax.set_xlabel("(1 / aperture radius) $(m^{{-1}})$")
    ax.set_ylabel("central spot radius (m)")
    ax.set_title("Airy disc radii")
    ax.legend()

    plt.savefig(os.path.join(SAVE_DIR, "airy.pdf"))


def plot_rect_sizes(data):
    xs = 2/data[1]      # inverse aperture size/2
    ys = data[3]        # image size
    dys = data[4]       # image size error

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # plot the data
    fig, ax = plt.subplots(figsize=FIGSIZE)
    ax.errorbar(xs, ys, yerr=dys, fmt='+', label="data", markersize=FONTSIZE)

    # fit a line
    coefs = np.polyfit(xs, ys, 1, w=1/dys)
    fit_label = FIT_LABEL.format(coefs[0], coefs[1])
    print("Rectangular mirror fit coefs, largest power first: ", coefs)
    ax.plot(xs, np.polyval(coefs, xs), '-', label=fit_label)

    # label the plot
    ax.set_xlabel("(1 / aperture half-width) $(m^{{-1}})$")
    ax.set_ylabel("first minimum (m)")
    ax.set_title("Rectangular aperture image size")
    ax.legend()

    plt.savefig(os.path.join(SAVE_DIR, "rect.pdf"))


def plot_amplitudes(data):
    xs = np.power(data[1], 2)       # square aperture size
    ys = data[2]/np.min(data[2])    # central amplitude

    # formatting
    matplotlib.rcParams.update({'font.size': FONTSIZE})

    # plot
    fig, ax = plt.subplots(figsize=FIGSIZE)
    ax.plot(xs, ys, '+', label="data", markersize=FONTSIZE)

    # label the plot
    ax.set_xlabel("(aperture size)$^2$ $(m^2)$")
    ax.set_ylabel("central amplitude (arbitrary)")
    ax.set_title("Central amplitude of circular aperture")
    # ax.legend()

    plt.savefig(os.path.join(SAVE_DIR, "amp.pdf"))


if __name__ == "__main__":
    print_warning([SIZES_CONFIG, AMP_CONFIG, RECT_CONFIG])

    os.makedirs(SAVE_DIR, exist_ok=True)

    plot_sizes(read_data(SIZES_CONFIG))
    plot_amplitudes(read_data(AMP_CONFIG))
    plot_rect_sizes(read_data(RECT_CONFIG))
    plt.show()