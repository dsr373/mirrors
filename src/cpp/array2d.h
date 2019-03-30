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

/** Type that takes complex argument and returns real number.
 * These are functions such as abs, real, imag, arg, etc...
 */
using complex_to_real = double (*)(complex<double>);

/** THE class that stores a 2D nx by ny array of complex<double> numbers
 * internally represented as a 1D array of length (nx*ny). It offers access
 * to elements in mutable and immutable ways, (approximate) equality comparison.
 * 
 * NB it doesn't follow the rule of 3 for classes having pointer members.
 * This means that the (compiler-generated) copy constructor will not deep-copy
 * the data stored within, but rather just copy the pointer arr. This is done
 * on purpose to save time and memory when deep-copying isn't necessary.
 */
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
    void print_prop(complex_to_real fun, FILE * out_file) const;

    Array2d deep_copy() const;
    Array2d transpose() const;

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