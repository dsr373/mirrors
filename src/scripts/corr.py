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

X_LABEL = "Correlation length $(m)$"


def plot_runs_v_lc(data_dict, x_idx, y_idx, dy_idx=None, runs=None, x_label=X_LABEL, y_label=None, x_log=False, y_log=False):
    """
    The data is grouped by group_data into runs with different RMS errors
    We want to plot a dataset per run, and need to accumulate each run first
    x_idx, y_idx, dy_idx are the indices of x, y and errorbar columns respectively
    runs is a list of indices of which runs to plot (in the order of increasing key)
    """

    # check params for None's
    if runs is None:
        runs = range(len(data_dict))
    if y_label is None:
        y_label = ""

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # open the plot
    fig, ax = plt.subplots(figsize=FIGSIZE)
    
    # iterate over the separate runs -- the key is RMS of phase
    all_keys = sorted(list(data_dict.keys()))
    run_keys = [key for i, key in enumerate(data_dict.keys()) if i in runs]

    for key in run_keys:
        label =  "$\\sigma_\\epsilon = {:.2f} \\pi$".format(key / np.pi)

        acc = accumulate_data(data_dict[key], x_idx, y_idx, dyidx=dy_idx)

        xs = np.array(acc[0])       # correlation length
        ys = np.array(acc[1])       # fwhp
        dys = np.array(acc[2])      # error

        ax.errorbar(xs, ys, yerr=dys, fmt='+', label=label, markersize=FONTSIZE)

        # label the plot
        ax.set_xlabel(x_label)
        ax.set_ylabel(y_label)
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
    # they will be rows 16 and 17
    data = np.append(data, [fwhp_row], axis=0)
    data = np.append(data, [fwhp_sigma_row], axis=0)

    # here the 1% falloff distances are aggregated into an average and error
    falloff_distances = [data[12], data[13], data[14], data[15]]
    dist_row = np.mean(np.abs(falloff_distances), axis=0)
    dist_sigma_row = np.std(np.abs(falloff_distances), axis=0)
    # this will be rows 18 and 19
    data = np.append(data, [dist_row], axis=0)
    data = np.append(data, [dist_sigma_row], axis=0)

    # first, group by the RMS phase error
    sig_eps_groups = group_data(data, 4)
    
    # plot the FWHP versus correlation length
    fwhp_label = "Full width at half power $(m^{{-1}})$"
    plot_runs_v_lc(sig_eps_groups, 6, 16, dy_idx=17, runs=[0, 1, 4], y_label=fwhp_label)
    
    # plot the 1% falloff distance vs correlation length
    falloff_label = "1% falloff distance $(m^{{-1}})$"
    plot_runs_v_lc(sig_eps_groups, 6, 18, dy_idx=19, runs=[0, 1, 4, 7], y_label=falloff_label, x_log=True, y_log=True)

    # plot central amplitude vs correlation length
    amp_label = "Central amplitude (arb. units)"
    plot_runs_v_lc(sig_eps_groups, 6, 11, dy_idx=None, runs=[0, 1, 4, 7], y_label=amp_label)
    
    plt.show()