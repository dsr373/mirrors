#include<cstdio>
#include<cmath>
#include<complex>
#include<cstring>

#include<gsl/gsl_sf_trig.h>
#include<fftw3.h>

#include "array2d.h"
#include "util.h"

using namespace std;

// power of 2
#define N (1<<12)
#define INFO_OUT true

int main(int argc, char * argv[]) {
    // open logger
    Logger main_log(stdout, "main.cpp", INFO_OUT);

    // parse command line config
    main_log("Started");
    Config conf(argc, argv);
    main_log("Configured");

    // declarations
    Array2d in(N, N);
    Array2d out(N, N);
    fftw_plan p;

    // plan
    main_log("Planning...");
    p = fftw_plan_dft_2d(N, N, in.ptr(), out.ptr(), FFTW_FORWARD, FFTW_MEASURE);

    // fill in the input
    main_log("Initializing input...");
    double lx = 10, ly = 10;
    vector<double> xs = coords(lx, N);
    vector<double> ys = coords(ly, N);
    vector<double> fx = fftshift(fftfreq(N, lx/(double)N));
    vector<double> fy = fftshift(fftfreq(N, ly/(double)N));

    circular(in, xs, ys, {0.1});

    // execute
    main_log("Executing...");
    fftw_execute(p);
    
    // print arrays
    main_log("Writing Output...");
    Array2d out_f = fftshift(out);
    Limits lims_in = in.find_interesting(myabs, 0.0, 1e-2);
    Limits lims_out = out_f.find_interesting(myabs, 0.0, 1e-2);

    // main_log((string("in limits: ") + lims_to_str(lims_in)).c_str());
    // main_log((string("out limits: ") + lims_to_str(lims_out)).c_str());

    print_lim_array(conf.in_filep, myabs, in, xs, ys, lims_in);
    print_lim_array(conf.out_filep, myabs, out_f, fx, fy, lims_out);

    main_log("Done. Cleaning up...");
    // clean up and exit
    fftw_destroy_plan(p);

    main_log("Done. Exiting.");
    return 0;
}
