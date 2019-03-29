#ifndef ARRAY2D
#define ARRAY2D

#include<cstdio>
#include<complex>
#include<fftw3.h>
using namespace std;

using complex_to_real = double (*)(complex<double>);
//TODO: use that to define printing a property of the Array2d: print_prop(Array2d a, complex_to_real fun);

class Array2d {
private:
    int nx, ny;
    complex<double> * arr;

public:
    Array2d(int size_x, int size_y);
    ~Array2d();
    complex<double> * operator[](int ix);
    complex<double> operator()(int ix, int iy);
    fftw_complex * ptr();
    void print_prop(complex_to_real fun, FILE * out_file);
};

#endif