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
FILENAME = "rand.txt"
N_DIFF_SHAPES = 30
N_REPEAT = 10

NX, NY = 2**13, 2**13
DATA_PREFIX = os.path.join(DATA_DIR, "rand")
TASKS = "params fwhp central_amplitude"
REL_SENS, ABS_SENS = 0, 0
N_SHAPES = N_DIFF_SHAPES * N_REPEAT

TYPE = "rand_errors"
LX, LY = 512, 512

################ leave this ################
with open(os.path.join(CONF_DIR, FILENAME), "w") as fout:
    write_option(fout, "nx", 2**13)
    write_option(fout, "ny", 2**13)
    write_option(fout, "prefix", DATA_PREFIX)
    write_option(fout, "tasks", TASKS)
    write_option(fout, "rel_sens", REL_SENS)
    write_option(fout, "abs_sens", ABS_SENS)
    write_option(fout, "n_shapes", N_SHAPES)

    for i in range(N_DIFF_SHAPES):
        for j in range(N_REPEAT):
            fout.write("\n")
            write_option(fout, "type", TYPE)
            write_option(fout, "lx", LX)
            write_option(fout, "ly", LY)
            ################ and change this: ################
            r = 6           # telescope radius
            sig = 3         # taper
            r_int = 0       # interior hole
            s_eps = np.pi / N_DIFF_SHAPES * (i+1)        # noise std dev
            seed = np.random.randint(1, 32e3)            # rng seed
            write_option(fout, "params", "{: 5.5f} {: 5.5f} {: 5.5f} {: 5.5f} {: d}".format(r, sig, r_int, s_eps, seed))
