# utilities used for reading config files and such

import re
import numpy as np

SEP = "="*30

def print_warning(configs):
    print(SEP)
    print("Warning: for the script to run correctly, data must be produced by running the executable on: ")
    for conf in configs:
        print(conf)
    print("If you've done that already, ignore this message")
    print(SEP)

def extract_value(line):
    return re.split(r'\s+=\s+', line)[-1].strip()

def read_data(config_filename):
    """ Read the data file created by running some config file """
    with open(config_filename) as confin:
        prefix = None     # some default that must change
        lines = confin.readlines()
        for line in lines:
            if line.startswith("prefix"):
                prefix = extract_value(line)

    data_fname = prefix + "dat.txt"
    with open(data_fname) as fin:
        lines = fin.readlines()
        data = np.array([[float(x) for x in line.split()] for line in lines])
    return np.ndarray.transpose(data)