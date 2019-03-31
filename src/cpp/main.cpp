#include<cstdio>
#include<cmath>
#include<complex>
#include<cstring>

#include<gsl/gsl_sf_trig.h>
#include<fftw3.h>

#include "array2d.h"
#include "util.h"

using namespace std;

#define N 1001

int main(int argc, char * argv[]) {
    // parse command line config
    Config conf(argc, argv);

    // declarations
    Array2d in(N, N);
    Array2d out(N, N);
    fftw_plan p;

    // allocate
    p = fftw_plan_dft_2d(N, N, in.ptr(), out.ptr(), FFTW_FORWARD, FFTW_ESTIMATE);

    // fill in the input
    double lx = 10, ly = 10;
    vector<double> xs = coords(lx, N);
    vector<double> ys = coords(ly, N);
    vector<double> fx = fftshift(fftfreq(N, lx/(double)N));
    vector<double> fy = fftshift(fftfreq(N, ly/(double)N));

    circular(in, xs, ys, {0.1});

    // execute
    fftw_execute(p);
    // print arrays
    print_lim_array(conf.in_filep, myabs, in, xs, ys);
    print_lim_array(conf.out_filep, myabs, fftshift(out), fx, fy);

    // clean up and exit
    fftw_destroy_plan(p);
    return 0;
}
