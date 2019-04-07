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

def parse_config(filename):
    n_shapes = None
    prefix = None
    tasks = None

    with open(filename) as fin:
        lines = fin.readlines()
        for line in lines:
            if line.startswith("n_shapes"):
                n_shapes = int(extract_value(line))
            elif line.startswith("prefix"):
                prefix = extract_value(line)
            elif line.startswith("tasks"):
                tasks = extract_value(line)

    # extract what figures are produced for each shape
    figs = []
    for task in tasks.split():
        if task.startswith("print_"):
            figs.append(task[6:])
    
    return (n_shapes, prefix, figs)


def read_image(filename):
    """ Read image from a data file """
    
    print("Reading data from " + filename)
    with open(filename) as fin:
        lines = fin.readlines()
        data = [[float(x) for x in line.split()] for line in lines]
        xlim = data[0]
        ylim = data[1]  
        data = np.array(data[2:])
    return xlim, ylim, data


def read_data(config_filename):
    """ Read the data file created by running some config file """
    _, prefix, _ = parse_config(config_filename)

    data_fname = prefix + "dat.txt"
    with open(data_fname) as fin:
        lines = fin.readlines()
        data = np.array([[float(x) for x in line.split()] for line in lines])
    return np.ndarray.transpose(data)

def relim(data_shape, old_lim, new_lim):
    """ 
    Take a data array with known x and y limits
    and return the data between different x and y limits
    of course, the new limits should be included in the old ones
    Returns two tuples:
        (min_row, max_row, min_col, max_col)
    And then the coordinates:
        (xmin, xmax, ymin, ymax)
    """
    x1, x2, y1, y2 = old_lim
    xn1, xn2, yn1, yn2 = new_lim
    n_rows, n_cols = data_shape

    # the delta is the distance divided by the number of intervals
    dx = abs(x2 - x1) / (n_cols-1)
    dy = abs(y2 - y1) / (n_rows-1)
    
    # find the indices of the new coordinates
    min_col = int(np.ceil((xn1 - x1) / dx))
    max_col = int(np.ceil((xn2 - x1) / dx))

    min_row = int(np.ceil((yn1 - y1) / dy))
    max_row = int(np.ceil((yn2 - y1) / dy))

    xlim = (x1 + dx * min_col), (x1 + dx * (max_col-1))
    ylim = (y1 + dy * min_row), (y1 + dy * (max_row-1))

    return (min_row, max_row, min_col, max_col), xlim+ylim