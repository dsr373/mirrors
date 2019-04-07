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

def colour_plot(filename, title, colorbar=False, logdata=False, colormap=COLORMAP):
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    
    xlim, ylim, data = read_image(filename)
    if logdata:
        data = np.log(data)
    
    fig, ax = plt.subplots(figsize=FIGSIZE)
    im = ax.imshow(data, cmap=COLORMAP, interpolation='nearest', extent=(xlim+ylim))

    # draw colorbar
    if colorbar:
        fig.colorbar(im, ax=ax)

    ax.set_title(title)


SAVE_DIR = os.path.join("fig", "gauss")

if __name__ == "__main__":
    os.makedirs(SAVE_DIR, exist_ok=True)

    colour_plot(os.path.join("data", "gauss0out_abs.txt"), "Uniform illumination")
    plt.savefig(os.path.join(SAVE_DIR, "uniform.png"))

    colour_plot(os.path.join("data", "gauss1out_abs.txt"), "Gaussian illumination")
    plt.savefig(os.path.join(SAVE_DIR, "gaussian.png"))

    # uncomment this to automagically plot everything created by a certain config
    # otherwise do it manually with your own tweaks
    # n_shapes, prefix, figs = parse_config("config/config.txt")
    # for i in range(n_shapes):
    #     for fig in figs:
    #         filename = prefix + str(i) + fig + ".txt"
    #         title = "{:d} {:s}".format(i, fig.replace("_", " "))
    #         colour_plot(filename, title)
    plt.show()
