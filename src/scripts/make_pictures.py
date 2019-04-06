#!/usr/bin/python3

import re
import os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib

COLORMAP = matplotlib.cm.plasma
PHASE_COLORMAP = matplotlib.cm.hsv
FIGSIZE = (9, 9)
FONTSIZE = 16

def extract_value(line):
    return re.split(r'\s+=\s+', line)[-1].strip()

def parse_config(filename):
    n_shapes = None
    prefix = None
    tasks = None

    with open(filename) as fin:
        lines = fin.readlines()
        for line in lines:
            if line.startswith("n_shapes"):
                n_shapes = int(extract_value(line))
            elif line.startswith("prefix"):
                prefix = extract_value(line)
            elif line.startswith("tasks"):
                tasks = extract_value(line)

    # extract what figures are produced for each shape
    figs = []
    for task in tasks.split():
        if task.startswith("print_"):
            figs.append(task[6:])
    
    return (n_shapes, prefix, figs)


def readfile(filename):
    print("Reading data from " + filename)
    with open(filename) as fin:
        lines = fin.readlines()
        data = [[float(x) for x in line.split()] for line in lines]
        xlim = data[0]
        ylim = data[1]  
        data = np.array(data[2:])
    return xlim, ylim, data


def colour_plot(filename, title, colorbar=False, logdata=False, colormap=COLORMAP):
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    
    xlim, ylim, data = readfile(filename)
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
    # n_shapes, prefix, figs = parse_config(CONFIG)
    # for i in range(n_shapes):
    #     for fig in figs:
    #         filename = prefix + str(i) + fig + ".txt"
    #         title = "{:d} {:s}".format(i, fig.replace("_", " "))
    #         colour_plot(readfile(filename), title)
    plt.show()
