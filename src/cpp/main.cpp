#include<cstdio>
#include<cmath>
#include<complex>
#include<cstring>

#include<gsl/gsl_sf_trig.h>
#include<gsl/gsl_math.h>
#include<fftw3.h>

#include "array2d.h"
#include "util.h"

using namespace std;

#define N_THREADS 1

#define INFO_OUT true

int main(int argc, char * argv[]) {
    // open logger
    Logger main_log(stdout, "main.cpp", INFO_OUT);

    if(argc != 2) {
        main_log("Incorrect number of arguments. Provide one config file.");
        return 1;
    }

    // init fftw threads
    int threads_status = fftw_init_threads();
    if(threads_status == 0) {
        main_log("Thread initialisation failed!");
        return 1;
    }
    fftw_plan_with_nthreads(N_THREADS);
    main_log("Thread initialisation successful.");

    // parse command line config
    Config conf(argv[1]);
    main_log("Configured");

    // open data file;
    string data_filename = conf.out_prefix + "dat.txt";
    FILE * data_filep = fopen(data_filename.c_str(), "w");

    // declarations
    Array2d in(conf.nx, conf.ny);
    Array2d out(conf.nx, conf.ny);
    fftw_plan p;

    // plan
    main_log("Planning...");
    p = fftw_plan_dft_2d(conf.nx, conf.ny, in.ptr(), out.ptr(), FFTW_FORWARD, FFTW_MEASURE);

    // walk through the shapes
    for(unsigned int i = 0; i < conf.shapes.size(); i ++ ) {
        main_log("===== Shape " + to_string(i) + " =====");
        ShapeProperties sp = conf.shapes[i];

        fprintf(data_filep, "%u", i);

        // calculate x and y values for both in and out
        // the division by 2pi is because p and q are angular frequencies,
        // whereas the FFT produces number frequencies
        vector<double> xs = coords(sp.lx, conf.nx);
        vector<double> ys = coords(sp.ly, conf.ny);
        vector<double> ps = fftfreq(conf.nx, sp.lx/(double)conf.nx/(2*M_PI));
        vector<double> qs = fftfreq(conf.ny, sp.ly/(double)conf.ny/(2*M_PI));
        // the printing limits of the arrays, and flags whether they were calculated or not
        Limits in_lims, out_lims;

        // fill in the input
        main_log("Initializing input...");
        generators[sp.generator_key](in, xs, ys, sp.shape_params);

        main_log("Executing...");
        fftw_execute(p);

        main_log("Resolving tasks:");
        if(contains(conf.tasks, "params")) {
            // print shape parameters
            for(unsigned int ip = 0; ip < sp.shape_params.size(); ip ++ )
                fprintf(data_filep, "\t%lf", sp.shape_params[ip]);
        }
        if(contains(conf.tasks, "find_min")) {
            // print size of central spot and error
            ValueError<double> min_pos = find_first_min(myabs, out, ps);
            fprintf(data_filep, "\t%lf\t%lf", min_pos.val, min_pos.err);
        }
        if(contains(conf.tasks, "central_amplitude")) {
            // print absolute value of central spot
            fprintf(data_filep, "\t%lf", myabs(out(0, 0)));
        }
        fprintf(data_filep, "\n");

        // find interesting limits if printing is needed. This next bit is ugly, I know.
        if(any_begins_with(conf.tasks, "print_in")) {
            main_log("\tin limits");
            in_lims = in.find_interesting(myabs, conf.abs_sens, conf.rel_sens);
        }
        if(any_begins_with(conf.tasks, "print_out")) {
            // this screws up the out array:
            fftshift(out);
            main_log("\tout limits");
            out_lims = out.find_interesting(myabs, conf.abs_sens, conf.rel_sens);
        }

        // DO the array printing. 
        if(contains(conf.tasks, "print_in_abs")) {
            main_log("\tprint_in_abs");
            // print aperture amplitude
            string in_fname = conf.out_prefix + to_string(i) + "in_abs.txt";
            FILE * in_filep = fopen(in_fname.c_str(), "w");
            print_lim_array(in_filep, myabs, in, xs, ys, in_lims);
            fclose(in_filep);
        }
        if(contains(conf.tasks, "print_in_phase")) {
            main_log("\tprint_in_phase");
            // print aperture phase
            string in_fname = conf.out_prefix + to_string(i) + "in_phase.txt";
            FILE * in_filep = fopen(in_fname.c_str(), "w");
            print_lim_array(in_filep, myarg, in, xs, ys, in_lims);
            fclose(in_filep);
        }
        if(contains(conf.tasks, "print_out_abs")) {
            main_log("\tprint_out_abs");
            // print image amplitude
            string out_fname = conf.out_prefix + to_string(i) + "out_abs.txt";
            FILE * out_filep = fopen(out_fname.c_str(), "w");
            print_lim_array(out_filep, myabs, out, fftshift(ps), fftshift(qs), out_lims);
            fclose(out_filep);
        }
        if(contains(conf.tasks, "print_out_phase")) {
            main_log("\tprint_out_phase");
            // print image phase
            string out_fname = conf.out_prefix + to_string(i) + "out_phase.txt";
            FILE * out_filep = fopen(out_fname.c_str(), "w");
            print_lim_array(out_filep, myarg, out, fftshift(ps), fftshift(qs), out_lims);
            fclose(out_filep);
        }
    }

    main_log("Done. Cleaning up...");
    fftw_destroy_plan(p);
    fclose(data_filep);

    main_log("Done. Exiting.");
    return 0;
}
