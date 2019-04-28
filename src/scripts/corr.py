#!/usr/bin/python3

import os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib

from util import *

FONTSIZE = 18
CAPSIZE = 7
FIT_LABEL = "$y={:4.4f}x{:+4.4f}$"
FIT_INFO = "Slope: {:4.6f} +/- {:4.6f}\nIntercept: {:4.6f} +/- {:4.6f}"
FIGSIZE = (9, 9)


def plot_width_v_lc(data_dict):
    """
    The data is grouped by group_data into runs with different RMS errors
    We want to plot a dataset per run, and need to accumulate first
    """

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # open the plot
    fig, ax = plt.subplots(figsize=FIGSIZE)
    
    # iterate over the separate runs
    for i, key in enumerate(data_dict.keys()):
        label =  "$\\sigma_\\epsilon = {:.2f} \\pi$".format(key / np.pi)

        acc = accumulate_data(data_dict[key], 6, 16, dyidx=17)

        xs = np.array(acc[0])       # correlation length
        ys = np.array(acc[1])       # fwhp
        dys = np.array(acc[2])      # error

        ax.errorbar(xs, ys, yerr=dys, fmt='+', label=label, markersize=FONTSIZE)

        # label the plot
        ax.set_xlabel("Correlation length (m)")
        ax.set_ylabel("Full width at half power")
        ax.legend()

    # plt.savefig(os.path.join(SAVE_DIR, "depth.pdf"), bbox_inches="tight")


CONFIG1 = "config/corr_big.txt"
SAVE_DIR = os.path.join("fig", "corr")

if __name__ == "__main__":
    print_warning([CONFIG1])

    os.makedirs(SAVE_DIR, exist_ok=True)

    data = read_data(CONFIG1)

    # here is some pre-processing
    # i want to calculate the average of FWHP and the average of 1% fall-off distace
    # and put them into their own rows. Currently, the data is printed with the
    # x and y values separate, so here they get aggregated into one value

    # here the fhwp in x and y are aggregated into an average and error
    fwhp_row = np.mean([ data[7], data[9] ], axis=0)
    fwhp_sigma_row = np.std([ data[7], data[9] ], axis=0)
    fwhp_sigma_row = np.max([fwhp_sigma_row, data[8], data[10]], axis=0)

    data = np.append(data, [fwhp_row], axis=0)
    data = np.append(data, [fwhp_sigma_row], axis=0)

    # here the 1% falloff distances are aggregated into an average and error
    dist_row = np.mean([ data[12], data[13], data[14], data[15] ], axis=0)
    dist_sigma_row = np.std([ data[12], data[13], data[14], data[15] ], axis=0)
    
    data = np.append(data, [dist_row], axis=0)
    data = np.append(data, [dist_sigma_row], axis=0)

    # first, group by the RMS phase error
    by_sig_eps = group_data(data, 4)
    plot_width_v_lc(by_sig_eps)
    
    plt.show()