#!/usr/bin/python3
# Utility script to make config files programatically
# Change to your leisure

import os
import numpy as np

################ leave these alone ################
PARAM_STR = "{:s} = {}\n"

CONF_DIR = "config"
DATA_DIR = "data"
os.makedirs(CONF_DIR, exist_ok=True)
os.makedirs(DATA_DIR, exist_ok=True)

# write one key = value line
def write_option(fout, name, value):
    fout.write(PARAM_STR.format(name, value))

################ change these ################
FILENAME = "corr_sig.txt"

# write the RMS errors
sigmas = [(i+1)/12 * np.pi for i in range(12)]
print(sigmas)
# for each, write the correlation lengths
ls = [(i+1) * 0.25 for i in range(5)]
print(ls)

# repeat each this many times - to deal with the randomness
N_DIFF_SHAPES = len(ls) * len(sigmas)
N_REPEAT = 10

NX, NY = 2**13, 2**13
DATA_PREFIX = os.path.join(DATA_DIR, "corr_sig")
TASKS = "params fwhp fwhp_y central_amplitude out_lims"
REL_SENS, ABS_SENS = 0.01, 0

TYPE = "corr_errors"
LX, LY = 512, 512

################ leave this ################
with open(os.path.join(CONF_DIR, FILENAME), "w") as fout:
    write_option(fout, "nx", NX)
    write_option(fout, "ny", NY)
    write_option(fout, "prefix", DATA_PREFIX)
    write_option(fout, "tasks", TASKS)
    write_option(fout, "rel_sens", REL_SENS)
    write_option(fout, "abs_sens", ABS_SENS)
    write_option(fout, "n_shapes", N_DIFF_SHAPES * N_REPEAT)

    for s_eps in sigmas:
        for l_corr in ls:
            for _ in range(N_REPEAT):
                fout.write("\n")
                write_option(fout, "type", TYPE)
                write_option(fout, "lx", LX)
                write_option(fout, "ly", LY)
                ################ and change this: ################
                r = 6           # telescope radius
                sig = 3         # taper
                r_int = 0       # interior hole
                seed = np.random.randint(1, 32e3)        # rng seed
                write_option(fout, "params", "{: 5.5f} {: 5.5f} {: 5.5f} {: 5.5f} {:05d} {: 5.5f}".format(r, sig, r_int, s_eps, seed, l_corr))
