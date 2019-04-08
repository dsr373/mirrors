#!/usr/bin/python3
# Utility script to make config files programatically
# Change to your leisure

import os

# leave these alone
PARAM_STR = "{:s} = {}\n"

CONF_DIR = "config"
DATA_DIR = "data"
os.makedirs(CONF_DIR, exist_ok=True)
os.makedirs(DATA_DIR, exist_ok=True)

# write one key = value line
def write_option(fout, name, value):
    fout.write(PARAM_STR.format(name, value))

# change these
FILENAME = "gauss_amp.txt"

NX, NY = 2**13, 2**13
DATA_PREFIX = os.path.join(DATA_DIR, "gauss_amp")
TASKS = "params fwhp central_amplitude"
REL_SENS, ABS_SENS = 0, 0
N_SHAPES = 40

TYPE = "gaussian"
LX, LY = 512, 512

with open(os.path.join(CONF_DIR, FILENAME), "w") as fout:
    write_option(fout, "nx", 2**13)
    write_option(fout, "ny", 2**13)
    write_option(fout, "prefix", DATA_PREFIX)
    write_option(fout, "tasks", TASKS)
    write_option(fout, "rel_sens", REL_SENS)
    write_option(fout, "abs_sens", ABS_SENS)
    write_option(fout, "n_shapes", N_SHAPES)

    for i in range(N_SHAPES):
        fout.write("\n")
        write_option(fout, "type", TYPE)
        write_option(fout, "lx", LX)
        write_option(fout, "ly", LY)
        # and change this:
        r = 6
        sig = float(i+1) / 1.5
        write_option(fout, "params", "{: 5.5f} {: 5.5f}".format(r, sig))
