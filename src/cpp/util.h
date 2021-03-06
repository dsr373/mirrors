#ifndef MYUTIL
#define MYUTIL

#include<cstdio>
#include<cstring>
#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include<complex>
#include<mutex>

using namespace std;

#define EPS 1e-6
#define DBL_EQ(a, b) (abs(a-b) < EPS)

#define CONV_KEY "corr_errors"

// a mutex locks thread execution to only allow one thread at a time to access a resource
// here it's needed because FFTW only allows one thread to plan FFTs at a time
extern mutex planner_mtx;

/** Type that takes complex argument and returns real number.
 * These are functions such as abs, real, imag, arg, etc...
 */
using complex_to_real = double (*)(complex<double>);
extern complex_to_real myabs, myarg, myre, complexness;

/** Find if s exists in a vector of strings */
bool contains(const vector<string> &v, const char * s);
/** Find if any string in a vector begins with s */
bool any_begins_with(const vector<string> &v, const char * s);

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
 * tasks is the list of things to do with each shape
 * out_prefix is a prefix for the files where to print data
 * shapes is a vector of shapes to process
 * abs_sens and rel_sens are the sensitivities at printing. Use 0 to print everything.
 * convolution is a flag describing whether a convolution in the input array is needed. If yes, we'll need a second FFT plan for transforming backwards, because convolution is done by multiplying the FFT results.
 */
struct Config {
    Config(const char * filename);

    string out_prefix;
    vector<string> tasks;
    vector<ShapeProperties> shapes;

    int nx, ny;
    double abs_sens, rel_sens;
};


/**
 * My own utility logger, than can be turned on or off
 * whenever. It writes to the given file pointer, which can
 * be stdout as well, and prepends its name.
 */
struct Logger {
    FILE * filep;
    string name;
    bool enabled;
    Logger(FILE * filep, const char * name, bool enabled);
    void write(const char * message) const;
    void operator()(const char * message) const;
    void operator()(const string message) const;
};


template <typename T>
struct ValueError {
    T val;
    T err;
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