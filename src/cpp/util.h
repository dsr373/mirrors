#ifndef MYUTIL
#define MYUTIL

#include<cstdio>
#include<cstring>
#include<vector>
#include<complex>

using namespace std;

#define EPS 1e-6
#define DBL_EQ(a, b) (abs(a-b) < EPS)

/** Type that takes complex argument and returns real number.
 * These are functions such as abs, real, imag, arg, etc...
 */
using complex_to_real = double (*)(complex<double>);
extern complex_to_real myabs, myarg, myre;

/**
 * Struct containing the configuration of the program.
 * in_filep is a file pointer for the input array;
 * out_filep is a file pointer for the output array;
 */
struct Config {
    FILE * in_filep;
    FILE * out_filep;
    Config(int argc, char * argv[]);
    ~Config();
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


/**
 * Calculate the coordinates of n evenly distributed points between -l/2 and l/2
 */
vector<double> coords(double l, int n);

#endif