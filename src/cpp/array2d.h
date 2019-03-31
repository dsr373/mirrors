#ifndef ARRAY2D
#define ARRAY2D

#include<cstdio>
#include<complex>
#include<vector>
#include<algorithm>

#include<fftw3.h>

#include "util.h"
using namespace std;


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
 * Print the limits in two directions of the 2d array, then the array itself,
 * in standard formatted way.
 */
void print_lim_array(FILE * filep, complex_to_real fun, const Array2d &a, const vector<double> &xs, const vector<double> &ys);

/**
 * Type of function that writes and aperture an aperture given
 * the list of x and y coordinates and a vector of parameters.
 */
using aperture_generator = int (*)(Array2d&, const vector<double>&, const vector<double>&, const vector<double>&);

int circular(Array2d& in, const vector<double>& xs, const vector<double>& ys, const vector<double>& params);
int rectangle(Array2d& in, const vector<double>& xs, const vector<double>& ys, const vector<double>& params);
int double_slit(Array2d& in, const vector<double>& xs, const vector<double>& ys, const vector<double>& params);

#endif