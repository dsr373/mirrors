#include<cstdio>
#include<cmath>
#include<complex>
#include<queue>
#include<thread>

#include<gsl/gsl_sf_trig.h>
#include<gsl/gsl_math.h>
#include<fftw3.h>

#include "array2d.h"
#include "util.h"

using namespace std;

#define N_THREADS 1
#define N_WORKERS 2

#define INFO_OUT true

/** This block defines a struct that has an index and a string;
 * It's meant to hold a line of data to be printed to the data file.
 * The comparator is used to sort data lines by the index in the priority queue, so that
 * if we use multiple workers, the data lines are still printed in order when
 * main() does the dequeueing.
 */
struct DataLine{
    unsigned int idx;
    string line;
};

auto dlcmp = [](DataLine a, DataLine b) { return a.idx > b.idx; };
priority_queue<DataLine, vector<DataLine>, decltype(dlcmp)> dataq(dlcmp);


/** Process the shapes between start and end (inclusive, exclusive) in the config.
 * n_proc is the processor number, used in the logger name for debugging
 * Push the data results to the data queue; Array printing is handled here;
 */
void shapes_worker(const Config& conf, unsigned int n_proc, unsigned int start, unsigned int end) {
    // init a logger for each processor
    string logname = "work_" + to_string(n_proc);
    Logger proc_log(stdout, logname.c_str(), INFO_OUT);

    proc_log("Started on shapes " + to_string(start) + " to " + to_string(end));

    // declarations
    Array2d in(conf.nx, conf.ny);
    Array2d out(conf.nx, conf.ny);
    fftw_plan plan;

    // plan
    planner_mtx.lock();
    proc_log("Locked. Planning...");
    plan = fftw_plan_dft_2d(conf.nx, conf.ny, in.ptr(), out.ptr(), FFTW_FORWARD, FFTW_MEASURE);
    planner_mtx.unlock();
    proc_log("Unlocked. Planning done.");

    for(unsigned int shape_idx = start; shape_idx < end; shape_idx ++ ) {
        proc_log("===== Shape " + to_string(shape_idx) + " =====");
        ShapeProperties sp = conf.shapes[shape_idx];

        // construct new data line
        DataLine dl{shape_idx, to_string(shape_idx)};

        // calculate x and y values for both in and out
        // the division by 2pi is because p and q are angular frequencies,
        // whereas the FFT produces number frequencies
        vector<double> xs = coords(sp.lx, conf.nx);
        vector<double> ys = coords(sp.ly, conf.ny);
        vector<double> ps = fftfreq(conf.nx, sp.lx/(double)conf.nx/(2*M_PI));
        vector<double> qs = fftfreq(conf.ny, sp.ly/(double)conf.ny/(2*M_PI));
        // the printing boundaries of the arrays
        Limits in_lims, out_lims;

        // fill in the input
        proc_log("Initializing input...");
        generators[sp.generator_key](in, xs, ys, sp.shape_params);

        proc_log("Executing...");
        fftw_execute(plan);

        proc_log("Resolving tasks:");
        if(contains(conf.tasks, "params")) {
            // print shape parameters
            for(unsigned int ip = 0; ip < sp.shape_params.size(); ip ++ )
                dl.line += "\t" + to_string(sp.shape_params[ip]);
        }
        if(contains(conf.tasks, "find_min")) {
            // print size of central spot and error
            ValueError<double> min_pos = find_first_min(myabs, out, ps);
            dl.line += "\t" + to_string(min_pos.val) + "\t" + to_string(min_pos.err);
        }
        if(contains(conf.tasks, "fwhp")) {
            // print coordinate of full-width at half-power along horizontal.
            // times by 2 for FULL width (function gives half width)
            ValueError<double> res = hwhp(out, ps);
            dl.line += "\t" + to_string(res.val * 2) + "\t" + to_string(res.err * 2);
        }
        if(contains(conf.tasks, "fwhp_y")) {
            // print coordinate of FWHP along vertical
            ValueError<double> res = hwhp(out, qs, true);
            dl.line += "\t" + to_string(res.val * 2) + "\t" + to_string(res.err * 2);
        }
        if(contains(conf.tasks, "central_amplitude")) {
            // print absolute value of central spot
            dl.line += "\t" + to_string(myabs(out(0, 0)));
        }
        if(contains(conf.tasks, "in_phase_stat")) {
            // print the mean and RMS of phase errors in input array
            ValueError<double> stat = mean_stddev(myarg, in, xs, ys, sp.shape_params[0]);
            dl.line += "\t" + to_string(stat.val) + "\t" + to_string(stat.err);
        }

        // find interesting limits if printing is needed. This next bit is ugly, I know.
        if(any_begins_with(conf.tasks, "print_in")) {
            // look for in limits
            proc_log("\tin limits");
            in_lims = in.find_interesting(myabs, conf.abs_sens, conf.rel_sens);
        }

        if(any_begins_with(conf.tasks, "print_out") || contains(conf.tasks, "out_lims")) {
            // this screws up out
            proc_log("\tfftshift(out)");
            fftshift(out);
            ps = fftshift(ps); qs = fftshift(qs);
            
            // look for out limits
            proc_log("\tout limits");
            out_lims = out.find_interesting(myabs, conf.abs_sens, conf.rel_sens);
        }

        if(contains(conf.tasks, "out_lims")) {
            // record the boundaries of the image that are above the given sensitivity
            // reminder: lims = {imin, imax, jmin, jmax}
            int imin = out_lims[0], imax = out_lims[1];
            int jmin = out_lims[2], jmax = out_lims[3];
            double p1 = ps[jmin], p2 = ps[jmax - 1];
            double q1 = qs[imin], q2 = qs[imax - 1];

            dl.line += "\t" + to_string(p1) + "\t" + to_string(p2);
            dl.line += "\t" + to_string(q1) + "\t" + to_string(q2);
        }

        // DO the array printing. 
        if(contains(conf.tasks, "print_in_abs")) {
            proc_log("\tprint_in_abs");
            // print aperture amplitude
            string in_fname = conf.out_prefix + to_string(shape_idx) + "in_abs.txt";
            FILE * in_filep = fopen(in_fname.c_str(), "w");
            print_lim_array(in_filep, myabs, in, xs, ys, in_lims);
            fclose(in_filep);
        }
        if(contains(conf.tasks, "print_in_phase")) {
            proc_log("\tprint_in_phase");
            // print aperture phase
            string in_fname = conf.out_prefix + to_string(shape_idx) + "in_phase.txt";
            FILE * in_filep = fopen(in_fname.c_str(), "w");
            print_lim_array(in_filep, myarg, in, xs, ys, in_lims);
            fclose(in_filep);
        }
        if(contains(conf.tasks, "print_out_abs")) {
            proc_log("\tprint_out_abs");
            // print image amplitude
            string out_fname = conf.out_prefix + to_string(shape_idx) + "out_abs.txt";
            FILE * out_filep = fopen(out_fname.c_str(), "w");
            print_lim_array(out_filep, myabs, out, ps, qs, out_lims);
            fclose(out_filep);
        }
        if(contains(conf.tasks, "print_out_phase")) {
            proc_log("\tprint_out_phase");
            // print image phase
            string out_fname = conf.out_prefix + to_string(shape_idx) + "out_phase.txt";
            FILE * out_filep = fopen(out_fname.c_str(), "w");
            print_lim_array(out_filep, myarg, out, ps, qs, out_lims);
            fclose(out_filep);
        }
        dataq.push(dl);
    }

    proc_log("Done. Cleaning up...");
    fftw_destroy_plan(plan);
}


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

    // Multithread the shape processing
    unsigned int shapes_per_thread = conf.shapes.size() / N_WORKERS;
    vector<thread> worker_threads;

    main_log("Spawning worker threads");
    for(unsigned int i_th = 0; i_th < N_WORKERS; i_th ++ ) {
        unsigned int start = i_th * shapes_per_thread;
        unsigned int end = (i_th + 1) * shapes_per_thread;
        // the last worker has to finish the shapes
        if(i_th == N_WORKERS - 1) end = conf.shapes.size();

        if(start < end)
            // only start workers if they have something to do
            worker_threads.push_back(thread(shapes_worker, conf, i_th, start, end));
    }

    // join everything when it's done
    for(vector<thread>::iterator th = worker_threads.begin(); th != worker_threads.end(); th++ )
        th->join();

    main_log("Writing data results");
    // open data file;
    string data_filename = conf.out_prefix + "dat.txt";
    FILE * data_filep = fopen(data_filename.c_str(), "w");
    
    // print the data
    DataLine dl;
    while(!dataq.empty()) {
        dl = dataq.top();
        fprintf(data_filep, "%s\n", dl.line.c_str());
        dataq.pop();
    }
    fclose(data_filep);

    main_log("Done. Exiting.");
    return 0;
}
