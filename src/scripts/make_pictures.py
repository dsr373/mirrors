#!/usr/bin/python3

import re
import os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib

from util import *

COLORMAP = matplotlib.cm.inferno
PHASE_COLORMAP = matplotlib.cm.bwr
FIGSIZE = (9, 8)
FONTSIZE = 16

def colour_plot(data, limits, title, colorbar=False, colormap=COLORMAP):
    matplotlib.rcParams.update({'font.size': FONTSIZE})
        
    fig, ax = plt.subplots(figsize=FIGSIZE)
    im = ax.imshow(data, cmap=colormap, interpolation='nearest', extent=limits)

    # draw colorbar
    if colorbar:
        fig.colorbar(im, ax=ax, format="% .2g")

    ax.set_title(title)


SAVE_DIR = os.path.join("fig", "error_pics")

if __name__ == "__main__":
    os.makedirs(SAVE_DIR, exist_ok=True)

    # colour_plot(os.path.join("data", "gauss0out_abs.txt"), "Uniform illumination", colorbar=True)
    # plt.savefig(os.path.join(SAVE_DIR, "uniform.png"), bbox_inches='tight')

    # colour_plot(os.path.join("data", "gauss1out_abs.txt"), "Gaussian illumination", colorbar=True)
    # plt.savefig(os.path.join(SAVE_DIR, "gaussian.png"), bbox_inches='tight')

    # uncomment this to automagically plot everything created by a certain config
    # otherwise do it manually with your own tweaks
    n_shapes, prefix, figs = parse_config("config/errors.txt")
    for i in range(n_shapes):
        for fig in figs:
            filename = prefix + str(i) + fig + ".txt"
            title = fig.replace("_", " ").replace("in", "mirror").replace("out", "image").replace("abs", "amplitude")

            # read the data
            xlim, ylim, data = read_image(filename)
            coord_lim = xlim+ylim
            
            # select the colormap
            if fig.endswith("phase"):
                colormap = PHASE_COLORMAP
            else:
                colormap = COLORMAP
            
            # relimit to make all the same size
            if fig.startswith("out"):
                idx_lim, coord_lim = relim(data.shape, xlim+ylim, (-4.1, 4.1, -4.1, 4.1))
                min_row, max_row, min_col, max_col = idx_lim
                data = np.sqrt(data[min_row:max_row, min_col:max_col])

            colour_plot(data, coord_lim, title, colorbar=True, colormap=colormap)
            
            # save the figure
            figname = filename.replace(".txt", ".png").replace("data/", "")
            plt.savefig(os.path.join(SAVE_DIR, figname), boox_inches="tight")
        plt.show()
