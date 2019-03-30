#include<cstdio>
#include<cmath>
#include<complex>
#include<cstring>

#include<gsl/gsl_sf_trig.h>
#include<fftw3.h>

#include "array2d.h"
#include "util.h"

using namespace std;

#define N 1000

int main(int argc, char * argv[]) {
    // parse command line config
    Config conf;
    int status = configure(argc, argv, conf);
    if(status != 0) return status;

    // declarations
    Array2d in(N, N);
    Array2d out(N, N);
    fftw_plan p;

    // allocate
    p = fftw_plan_dft_2d(N, N, in.ptr(), out.ptr(), FFTW_FORWARD, FFTW_ESTIMATE);

    // fill in the input
    double lx = 16, ly = 10;
    double ax = 0.12, ay = 0.24;
    vector<double> xs = coords(lx, N);
    vector<double> ys = coords(ly, N);

    for(int i = 0; i < N; i++) 
        for(int j = 0; j < N; j++) {
            if(abs(xs[j]) < ax/2.0 && abs(ys[i]) < ay/2.0)
                in[i][j] = 1;
            else
                in[i][j] = 0;
    }

    // plan
    fftw_execute(p);
    // print arrays
    complex_to_real myabs = [](complex<double> z) -> double {return abs(z);};
    // complex_to_real myarg = [](complex<double> z) -> double {return arg(z);};
    in.print_prop(myabs, conf.in_filep);
    fftshift(out).print_prop(myabs, conf.out_filep);

    // clean up and exit
    fftw_destroy_plan(p);
    return 0;
}
