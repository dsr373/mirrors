#ifndef ARRAY2D
#define ARRAY2D

#include<cstdio>
#include<complex>
#include<vector>
#include<array>
#include<string>

#include<fftw3.h>

#include "util.h"
using namespace std;

#define PRINT_FORMAT "% 6.5f\t"

// type containing i and j limits of interest in an Array2d
using Limits = array<int, 4>;
string lims_to_str(const Limits &l);

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
    int mult(const Array2d& a);
    int mult_each(complex<double> c);
    int divide_each(complex<double> c);
    friend bool operator==(const Array2d &a, const Array2d &b);
    
    fftw_complex * ptr();
    Limits find_interesting(complex_to_real fun, double abs_sens, double rel_sens) const;
    void print_prop(complex_to_real fun, FILE * out_file) const;
    void print_prop(complex_to_real fun, const Limits &lim, FILE * out_file) const;
    
    int copy_into(Array2d &a) const;

    friend void fftshift(Array2d &a);
};

/**
 * Find the first minimum of fun(z) along the horizontal axis of a
 * and the associated error
 */
ValueError<double> find_first_min(complex_to_real fun, const Array2d &a, const vector<double> &xs);

/**
 * Find the x-coordinate of the first half-power point and the error
 */
ValueError<double> hwhp(const Array2d &a, const vector<double> &xs);

/**
 * Calculate the mean and standard deviation of fun within a given radius
 */
ValueError<double> mean_stddev(complex_to_real fun, const Array2d &a, const vector<double>& xs, const vector<double>& ys, double radius);

/**
 * Print the limits in two dihections of the 2d array, then the array itself,
 * in standard formatted way. 
 * Only print stuff within x and y limits given by lims.
 */
void print_lim_array(FILE * filep, complex_to_real fun, const Array2d &a, const vector<double> &xs, const vector<double> &ys, const Limits &lims);

/**
 * Type of function that writes and aperture an aperture given
 * the list of x and y coordinates and a vector of parameters.
 */
using aperture_generator = int (*)(Array2d& arr, const vector<double>& xs, const vector<double>& ys, const vector<double>& params);

/** map the name found in config files to the actual function pointer
 * for dynamically choosing which functions to run
 */
extern map<string, aperture_generator> generators;

#endif