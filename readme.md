# Table of contents
- [Table of contents](#table-of-contents)
- [What?](#what)
- [How is it made?](#how-is-it-made)
- [How to run it](#how-to-run-it)
  - [Pre-requisites](#pre-requisites)
  - [Compilation](#compilation)
  - [Running](#running)
- [Config files](#config-files)
  - [Syntax](#syntax)
  - [Tasks](#tasks)
  - [Aperture types](#aperture-types)
- [Plotting](#plotting)

# What?

A university project to study the behaviour of imperfect telescope mirrors.

# How is it made?

The hard lifting is done in C++, using the `fftw3` library. Functions of the mirror surface are turned into resulting images using Discrete Fourier Transforms (DFT). Plotting is done in Python with `matplotlib`, with supporting calculations using `numpy`.

# How to run it

## Pre-requisites

To compile and run the program, you need the following:

* a C++ compiler. g++ is used by default
* GNU make
* Python3 installed and in PATH
* matplotlib and numpy installed with pip3

You also need the following C/C++ libraries:

```text
libfftw3-dev (with thread support)
libgsl-dev
libgslcblas0
```

On Ubuntu these are available in the default repositories and can be installed with `apt install`.

## Compilation

The first time, to make sure the workspace directories are set up correctly, run:

```bash
make all
```

If you change anything, recompile with: `make default` or just `make` for the program itself and `make test` for the tests.

## Running

You need to use one of the provided configuration files or write your own to run the program. For an explanation of these, read the next section. Given a file `config/file.txt`, run the program on it as:

```bash
bin/main.exe config/file.txt
```

N.B.: `.exe` is just a naming convention, it's not a Windows executable!

# Config files

## Syntax

These hold instructions for what mirror shapes the C++ program will process. They have a syntax that the program checks, and are made up of `key = value` lines. I will describe them as:

* key = `data type`: explanation

The options are, in order:

* nx = `integer`: number of rows in the array
* ny = `integer`: number of columns in the array
* prefix = `string`: prepended to the name of data files produced. Can include a directory
* tasks = `space-separated strings`: the things to do to each shape. Explained later.
* rel_sens = `float`: fraction of the maximum below which array elements won't be printed
* abs_sens = `float`: absolute value below which array elements won't be printed
* n_shapes = `integer`: number of shapes that follow

Then, for each shape:

* type = `string`: the aperture function
* lx = `float`: width in x over which aperture function is defined.
* ly = `float`: width in y over which aperture function is defined.
* params = `space-separated floats`: parameters to be passed to the aperture function. Things like the radius, taper, etc.

## Tasks

The possible tasks are:

* print_in_abs
* print_in_phase
* print_out_abs
* print_out_phase

These print the absolute value or complex arguments of the mirror (aperture) and image (diffracton pattern) respectively, within the limits where elements are larger than the sensitivities specified. Each of these creates a new file for each shape, with the following naming scheme:

```text
prefix + shape index + suffix + ".txt"
```

where the suffix is e.g. `in_abs` or `out_phase`.

USE WITH CARE: for large arrays, printing can get slower than the actual Fourier transform and use large amounts of disk space (1GB is not uncommon).

The following tasks all print to a single data file per config, named `<prefix>dat.txt`

* params: shape parameters
* fwhp: full width at half power
* find_min: find the first minimum along the horizontal, starting from the centre
* central_amplitude: print the central value in the image array (arbitrary units that depend of nx, ny and other factors)

## Aperture types

These are the available aperture types, and how the parameters are interpreted for each of them:

* `circular`: as the name says, and params[0] is the radius
* `rectangle`: self-explanatory again. params[0] and params[1] are the horizontal and vertical full widths
* `gaussian`: radially decaying amplitude. params[0] is radius, params[1] is decay length
* `gaussian_hole`: radially decaying, with a central hole. First 2 params as before, params[2] is the radius of the hole.
* `rand_errors`: radially decaying, with a central hole and random phase errors. first 3 params as before, params[3] is the square average of the phase errors (in radians), and params[4] (optional) is the seed to pass to the random number generator. NB that with the default seed the RNG will always produce the same numbers.

# Plotting

There is a plotting script in `src/scripts/` for each investigation performed by me. They are each documented in the comments.

`make_pictures.py` just creates pictures of whatever files you specify to it. It expects files as produced by print commands passed to the C++ program.