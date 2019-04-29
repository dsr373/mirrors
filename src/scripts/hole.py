#!/usr/bin/python3

import re
import os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib

from util import *

COLORMAP = matplotlib.cm.plasma
FIGSIZE = (12, 9)
FONTSIZE = 16

def colour_plot(data, limits, title, colorbar=False, logdata=False, colormap=COLORMAP):
    matplotlib.rcParams.update({'font.size': FONTSIZE})

    if logdata:
        data = np.log(data)

    fig, ax = plt.subplots(figsize=FIGSIZE)
    im = ax.imshow(data, cmap=COLORMAP, interpolation='nearest', extent=limits)

    # draw colorbar
    if colorbar:
        fig.colorbar(im, ax=ax, format="% .2g")

    ax.set_title(title)


SAVE_DIR = os.path.join("fig", "hole")

if __name__ == "__main__":
    os.makedirs(SAVE_DIR, exist_ok=True)

    """
    What I'm doing here: take read the amplitude and phase of the small and large
    mirror patterns, then compute the difference between them. Compare that
    to the mirror-with-hole pattern to see how different it is. 
    """

    # read in the large mirror pattern
    xlim, ylim, l_abs = read_image("data/hole0out_abs.txt")
    xlim, ylim, l_arg = read_image("data/hole0out_phase.txt")
    # make it complex
    l_true = np.multiply(l_abs, np.exp(1j * l_arg))

    # read in the small mirror pattern
    xs, ys, s_abs = read_image("data/hole1out_abs.txt")
    (row_min, row_max, col_min, col_max), newlims = relim(s_abs.shape, xs+ys, xlim+ylim)
    s_abs = s_abs[row_min:row_max, col_min:col_max]

    xs, ys, s_arg = read_image("data/hole1out_phase.txt")
    s_arg = s_arg[row_min:row_max, col_min:col_max]
    # make complex
    s_true = np.multiply(s_abs, np.exp(1j * s_arg))

    # take the difference
    diff = np.abs(l_true - s_true)

    # read the holed thing
    xs, ys, h_abs = read_image("data/hole2out_abs.txt");
    (row_min, row_max, col_min, col_max), newlims = relim(h_abs.shape, xs+ys, xlim+ylim)
    h_abs = h_abs[row_min:row_max, col_min:col_max]
    colour_plot(np.sqrt(h_abs), newlims, "Holed mirror image", colorbar=True)
    plt.savefig(os.path.join(SAVE_DIR, "image.png"), bbox_inches='tight')

    colour_plot(np.abs(diff - h_abs), newlims, "Holed mirror delta", colorbar=True)
    plt.savefig(os.path.join(SAVE_DIR, "delta2.png"), bbox_inches='tight')

    plt.show()
