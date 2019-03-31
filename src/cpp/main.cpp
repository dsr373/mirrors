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

    // // declarations
    Array2d in(conf.nx, conf.ny);
    Array2d out(conf.nx, conf.ny);
    fftw_plan p;

    // plan
    main_log("Planning...");
    p = fftw_plan_dft_2d(conf.nx, conf.ny, in.ptr(), out.ptr(), FFTW_FORWARD, FFTW_MEASURE);

    // fill in the input
    for(unsigned int i = 0; i < conf.shapes.size(); i ++ ) {
        main_log(("Loop " + to_string(i)).c_str());
        main_log("Initializing input...");
        ShapeProperties sp = conf.shapes[i];

        // calculate x and y values for both in and out
        double dx = sp.lx / (double)conf.nx;
        double dy = sp.ly / (double)conf.ny;
        vector<double> xs = coords(sp.lx, conf.nx);
        vector<double> ys = coords(sp.ly, conf.ny);
        vector<double> fx = fftshift(fftfreq(conf.nx, dx));
        vector<double> fy = fftshift(fftfreq(conf.ny, dy));

        generators[sp.generator_key](in, xs, ys, sp.shape_params);

        main_log("Executing...");
        fftw_execute(p);

        main_log("Writing output...");
        // shift the output
        Array2d out_f = fftshift(out);
        
        // find areas of interest
        Limits lims_in = in.find_interesting(myabs, 0.0, 1e-2);
        Limits lims_out = out_f.find_interesting(myabs, 0.0, 5e-2);

        // find where to print
        string in_filename = conf.in_prefix + to_string(i) + ".txt";
        FILE * in_filep = fopen(in_filename.c_str(), "w");
        print_lim_array(in_filep, myabs, in, xs, ys, lims_in);

        string out_filename = conf.out_prefix + to_string(i) + ".txt";
        FILE * out_filep = fopen(out_filename.c_str(), "w");
        print_lim_array(out_filep, myabs, out_f, fx, fy, lims_out);
    }

    main_log("Done. Cleaning up...");
    fftw_destroy_plan(p);

    main_log("Done. Exiting.");
    return 0;
}
