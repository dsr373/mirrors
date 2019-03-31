#ifndef MYUTIL
#define MYUTIL

#include<cstdio>
#include<cstring>
#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include<complex>

using namespace std;

#define EPS 1e-6
#define DBL_EQ(a, b) (abs(a-b) < EPS)

/** Type that takes complex argument and returns real number.
 * These are functions such as abs, real, imag, arg, etc...
 */
using complex_to_real = double (*)(complex<double>);
extern complex_to_real myabs, myarg, myre;

/** Holds the properties of an aperture shape.
 * lx, ly are the lengths of the sides of the board
 * shape holds all the numbers necessary to make the shape, and it's passed
 * to the generator function.
 * generator_key is a key in the name-function map of aperture generators
 */
struct ShapeProperties {
    string generator_key;
    double lx, ly;
    vector<double> shape_params;
};


/**
 * Struct containing the configuration of the program.
 * nx and ny are the dimensions of the arrays used
 * in_prefix and out_prefix are prefixes for the files where to print arrays
 * shapes is a vector of shaped to process
 */
struct Config {
    Config(int argc, char * argv[]);

    string in_prefix, out_prefix;
    vector<ShapeProperties> shapes;

    int nx, ny;
};


struct Logger {
    FILE * filep;
    string name;
    bool enabled;
    Logger(FILE * filep, const char * name, bool enabled);
    void write(const char * message) const;
    void operator()(const char * message) const;
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