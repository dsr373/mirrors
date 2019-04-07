#!/usr/bin/python3

import re
import os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib

from util import *

COLORMAP = matplotlib.cm.plasma
FIGSIZE = (9, 9)
FONTSIZE = 16

def colour_plot(data, limits, title, colorbar=False, logdata=False, colormap=COLORMAP):
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    
    if logdata:
        data = np.log(data)
    
    fig, ax = plt.subplots(figsize=FIGSIZE)
    im = ax.imshow(data, cmap=COLORMAP, interpolation='nearest', extent=limits)

    # draw colorbar
    if colorbar:
        fig.colorbar(im, ax=ax)

    ax.set_title(title)


SAVE_DIR = os.path.join("fig", "hole")

if __name__ == "__main__":
    os.makedirs(SAVE_DIR, exist_ok=True)

    # draw the large mirror pattern
    xlim, ylim, data = read_image("data/hole0out_abs.txt")
    colour_plot(data, xlim+ylim, "Large Gaussian")

    # read in the small mirror pattern
    xs, ys, data = read_image("data/hole1out_abs.txt")
    # cut it to within the limits of the other
    (row_min, row_max, col_min, col_max), newlims = relim(data.shape, xs+ys, xlim+ylim)
    colour_plot(data[row_min:row_max, col_min:col_max], newlims, "Small Gaussian")
    
    plt.show()
