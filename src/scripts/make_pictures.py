#!/usr/bin/python3

import re
import os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib

COLORMAP = matplotlib.cm.plasma
FIGSIZE = (9, 9)
FONTSIZE = 16

SAVE_DIR = os.path.join("fig", "def")

def extract_value(line):
    return re.split(r'\s+=\s+', line)[-1].strip()

def parse_config(filename):
    n_shapes_line = None
    prefix_line = None

    with open(filename) as fin:
        lines = fin.readlines()
        for line in lines:
            if line.startswith("n_shapes"):
                n_shapes_line = line
            elif line.startswith("prefix"):
                prefix_line = line

    n_shapes = int(extract_value(n_shapes_line))
    prefix = extract_value(prefix_line)
    return (n_shapes, prefix)


def readfile(filename):
    with open(filename) as fin:
        lines = fin.readlines()
        data = [[float(x) for x in line.split()] for line in lines]
        xlim = data[0]
        ylim = data[1]  
        data = np.array(data[2:])
    return xlim, ylim, data


def colour_plot(readout, logdata=False):
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    xlim, ylim, data = readout
    if logdata:
        data = np.log(data)
    
    _, ax = plt.subplots(figsize=FIGSIZE)
    lims = xlim + ylim
    ax.imshow(data, cmap=COLORMAP, interpolation='nearest', extent=lims)


if __name__ == "__main__":
    n_shapes, prefix = parse_config("config/config.txt")
    os.makedirs(SAVE_DIR, exist_ok=True)
    for i in range(n_shapes):
        colour_plot(readfile(prefix + str(i) + "in.txt"))
        colour_plot(readfile(prefix + str(i) + "out.txt"))
        plt.savefig(os.path.join(SAVE_DIR, "out{:d}.png".format(i)))
        plt.show()
