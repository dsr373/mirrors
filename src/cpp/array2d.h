#ifndef ARRAY2D
#define ARRAY2D

#include<cstdio>
#include<complex>
#include<vector>

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

    friend Array2d fftshift(const Array2d &a);
};


/**
 * Generate the FT frequencies corresponding to n time-samples spaced by dt.
 * For even n, the positive frequencies are [1 .. n/2 - 1] / (n*dt)
 * For odd n, the positive frequencies are [1 .. (n-1)/2] / (n*dt)
 */
vector<double> fftfreq(int n, double dt=1.0);

/**
 * Create new vector where the zero-component freuqency is shifted to the middle
 */
template <typename T> vector<T> fftshift(const vector<T> &v) {
    int n = v.size();
    // apply shift1 to all values up to halfpoint
    // and shift2 to all values after halfpoint
    int halfpoint = n/2;
    int shift1 = (n+1)/2;
    int shift2 = -n/2;
    vector<T> shifted(n);
    
    for(int i = 0; i < halfpoint; i++)
        shifted[i] = v[i + shift1];
    
    for(int i = halfpoint; i < n; i++)
        shifted[i] = v[i + shift2];

    return shifted;
}

#endif