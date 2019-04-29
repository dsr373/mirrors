#!/usr/bin/python3

import os
import itertools

import numpy as np
import matplotlib.pyplot as plt
import matplotlib

from util import *

FONTSIZE = 18
CAPSIZE = 7
FIT_LABEL = "$y={:4.4f}x{:+4.4f}$"
FIT_INFO = "Slope: {:4.6f} +/- {:4.6f};\tIntercept: {:4.6f} +/- {:4.6f}"
FIGSIZE = (9, 9)

X_LABEL = "Correlation length $(m)$"

def add_means(data):
    """
    here is some pre-processing
    i want to calculate the average and error of x and y FWHP
    and the average and error of 1% fall-off distace in +/-x and +/-y
    and put them into their own rows.
    """
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

    return data


def get_keys_by_index(dict_keys, idx):
    """
    Sort the list of dict keys and return the ones of index idx
    """

    if idx is None:
        idx = range(len(dict_keys))

    sorted_keys = sorted(dict_keys)
    return [key for i, key in enumerate(sorted_keys) if i in idx]


def plot_runs_v_lc(data_dict, x_idx, y_idx, dy_idx=None, runs=None, x_label=X_LABEL, y_label=None, x_log=False, y_log=False):
    """
    The data is grouped by group_data into runs with different RMS errors
    We want to plot a dataset per run, and need to accumulate each run first
    x_idx, y_idx, dy_idx are the indices of x, y and errorbar columns respectively
    runs is a list of indices of which runs to plot (in the order of increasing key)
    """

    # check for None's
    if y_label is None:
        y_label = ""

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # open the plot
    fig, ax = plt.subplots(figsize=FIGSIZE)
    
    # iterate over the separate runs -- the key is RMS of phase
    run_keys = get_keys_by_index(data_dict.keys(), runs)

    for key in run_keys:
        label =  "$\\sigma_\\epsilon = {:.2f} \\pi$".format(key / np.pi)

        acc = accumulate_data(data_dict[key], x_idx, y_idx, dyidx=dy_idx)

        xs = np.array(acc[0])
        ys = np.array(acc[1])
        dys = np.array(acc[2])

        ax.errorbar(xs, ys, yerr=dys, fmt='+', label=label, markersize=FONTSIZE)

        # label the plot
        ax.set_xlabel(x_label)
        ax.set_ylabel(y_label)
        ax.legend()


def plot_falloff_v_lc(data_dict, runs=None):
    """
    Plots grouped data runs like the generic function, but with some extra tweaking
    and line fitting specific for this plot.
    """

    # set local vars
    x_idx = 6
    y_idx = 18
    dy_idx = 19

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # open the plot
    fig, ax = plt.subplots(figsize=FIGSIZE)
    
    # iterate over the separate runs -- the key is RMS of phase
    run_keys = get_keys_by_index(data_dict, runs)

    for run_no, key in enumerate(run_keys):
        label =  "$\\sigma_\\epsilon = {:.2f} \\pi$".format(key / np.pi)

        acc = accumulate_data(data_dict[key], x_idx, y_idx, dyidx=dy_idx)

        xs = np.log(acc[0])       # correlation length
        ys = np.log(acc[1])       # falloff distance
        dys = np.divide(acc[2], acc[1])      # relative error

        scatter = ax.errorbar(xs, ys, yerr=dys, fmt='+', label=label, markersize=FONTSIZE)[0]

        # get the x values for which to calculate the fit
        # this is empirical, and should be changed for new data
        if run_no in [0, 1]:
            # first 2 runs: take all the xs
            idxs = range(len(xs))
        elif run_no == 2:
            idxs = [i for i, x in enumerate(xs) if x > -1.5]
        else:
            idxs = [i for i, x in enumerate(xs) if x > -1.1]

        # fit a line and plot it
        coefs, covar = np.polyfit(xs[idxs], ys[idxs], 1, w=1/dys[idxs], cov=True)

        print(SEP)
        print("Run: sigma_eps = {: .2f} pi".format(key / np.pi))
        print(FIT_INFO.format(coefs[0], np.sqrt(covar[0, 0]),
              coefs[1], np.sqrt(covar[1, 1])))

        ax.plot(xs[idxs], np.polyval(coefs, xs[idxs]), '-', color=scatter.get_color())

        # label the plot
        ax.set_xlabel("log ({:s})".format(X_LABEL))
        ax.set_ylabel("log (1% decay distance $(m^{{-1}})$)")
        ax.legend()


def plot_amp_v_sig(data_dict, runs=None):
    """
    Now data is grouped by correlation length, and we want to plot central
    amplitude v RMS phase.
    """

    # local vars
    x_idx = 4
    y_idx = 11

    # formatting niceness
    matplotlib.rcParams.update({'font.size': FONTSIZE})
    matplotlib.rcParams.update({'errorbar.capsize': CAPSIZE})

    # open the plot
    fig, ax = plt.subplots(figsize=FIGSIZE)
    
    # iterate over the separate runs -- the key is correlation length
    run_keys = get_keys_by_index(data_dict.keys(), runs)

    for key in run_keys:
        label =  "$l_c = {:.2f}$".format(key)

        acc = accumulate_data(data_dict[key], x_idx, y_idx)

        norm = np.min(acc[1])   # normalization factor for the amplitude

        xs = np.power(np.divide(acc[0], 4 * np.pi), 2)  # phase error squared
        ys = np.log(acc[1] / norm)      # normalised amplitude 
        dys = np.divide(acc[2] / norm, acc[1])   # error in amplitude

        scatter = ax.errorbar(xs, ys, yerr=dys, fmt='+', label=label, markersize=FONTSIZE)[0]

        # find xs to use for line fit
        idxs = [i for i, x in enumerate(xs) if x < 0.03]

        # fit
        coefs, covar = np.polyfit(xs[idxs], ys[idxs], 1, w=1/dys[idxs], cov=True)

        print(SEP)
        print("Run: l_c = {:.4f}".format(key))
        print(FIT_INFO.format(coefs[0], np.sqrt(covar[0, 0]),
              coefs[1], np.sqrt(covar[1, 1])))

        ax.plot(xs[idxs], np.polyval(coefs, xs[idxs]), '-', color=scatter.get_color())

        # label the plot
        ax.set_xlabel("$(\\sigma_\\epsilon / \\lambda)^2$")
        ax.set_ylabel("log ($\\psi_0$ (arb. units))")
        ax.legend()


CONFIG1 = "config/corr_lc.txt"
CONFIG2 = "config/corr_sig.txt"
SAVE_DIR = os.path.join("fig", "corr")

if __name__ == "__main__":
    print_warning([CONFIG1, CONFIG2])
    os.makedirs(SAVE_DIR, exist_ok=True)

    # FIRST: consider runs of same sigma_err, different lc
    data = add_means(read_data(CONFIG1))
    # group by the RMS phase error
    sig_eps_groups = group_data(data, 4)
    
    # plot the FWHP versus correlation length
    fwhp_label = "Full width at half power $(m^{{-1}})$"
    plot_runs_v_lc(sig_eps_groups, 6, 16, dy_idx=17, y_label=fwhp_label)
    plt.savefig(os.path.join(SAVE_DIR, "fwhp.pdf"), bbox_inches="tight")
    
    # plot the 1% falloff distance vs correlation length
    plot_falloff_v_lc(sig_eps_groups)
    plt.savefig(os.path.join(SAVE_DIR, "dist.pdf"), bbox_inches="tight")

    # SECOND: consider runs of same lc, different sigma_err
    data = add_means(read_data(CONFIG2))
    # group data by the correlation length
    lc_groups = group_data(data, 6)
    # plot the central amplitude vs RMS phase
    plot_amp_v_sig(lc_groups)
    plt.savefig(os.path.join(SAVE_DIR, "amp.pdf"), bbox_inches="tight")
    
    plt.show()