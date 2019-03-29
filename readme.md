# What?
A university project to study the behaviour of imperfect telescope mirrors.

# How is it made?
The hard lifting is done in C++, using the fftw3 library. Functions of the mirror surface are turned into resulting images using Discrete Fourier Transforms (DFT).

# How to run it
First, you need fftw3 library installed. To compile, do:
```
make directories
make
```
Then run using:
```
bin/main.exe
```