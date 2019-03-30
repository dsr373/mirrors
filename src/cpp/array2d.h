#ifndef ARRAY2D
#define ARRAY2D

#include<cstdio>
#include<complex>
#include<vector>
#include<algorithm>

#include<fftw3.h>
using namespace std;

#define EPS 1e-6

#define DBL_EQ(a, b) (abs(a-b) < EPS)

using complex_to_real = double (*)(complex<double>);

class Array2d {
private:
    int nx, ny;
    complex<double> * arr;

public:
    Array2d(int size_x, int size_y);
    ~Array2d();

    complex<double> * operator[](int ix);
    complex<double> operator()(int ix, int iy) const;
    friend bool operator==(const Array2d &a, const Array2d &b);
    
    fftw_complex * ptr();
    void print_prop(complex_to_real fun, FILE * out_file);

    Array2d deep_copy() const;
    Array2d transpose();

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
    vector<T> shifted(v);
    int new_first = (n+1) / 2;
    
    rotate(shifted.begin(), shifted.begin() + new_first, shifted.end());

    return shifted;
}

#endif