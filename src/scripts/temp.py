#!/usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm

def readfile(filename):
    with open(filename) as fin:
        lines = fin.readlines()
        data = [[float(x) for x in line.split()] for line in lines]
        xlim = data[0]
        ylim = data[1]
        data = data[2:]
    return xlim, ylim, data


def colour_plot(readout):
    xlim, ylim, data = readout
    
    _, ax = plt.subplots()
    lims = xlim + ylim
    ax.imshow(data, cmap=cm.plasma, interpolation='nearest', extent=lims)


if __name__ == "__main__":
    colour_plot(readfile("data/in.txt"))
    colour_plot(readfile("data/out.txt"))
    plt.show()
