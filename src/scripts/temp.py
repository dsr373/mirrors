#!/usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm

def readfile(filename):
    return np.loadtxt(filename)


def colour_plot(data):
    plt.imshow(data, cmap=cm.plasma, interpolation='nearest')
    plt.show()


if __name__ == "__main__":
    colour_plot(readfile("data/in.txt"))