#!/usr/bin/python3

import re

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm

def parse_config(filename):
    with open(filename) as fin:
        lines = fin.readlines()
        n_shapes_line = lines[2]

        n_shapes = re.split(r'\s+=\s+', n_shapes_line)[-1]
        n_shapes = int(n_shapes)
    return (n_shapes, )

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
    n_shapes, = parse_config("config/config.txt")
    for i in range(n_shapes):
        colour_plot(readfile("data/in" + str(i) + ".txt"))
        colour_plot(readfile("data/out"+ str(i) + ".txt"))
        plt.show()
