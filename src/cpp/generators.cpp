#include "array2d.h"

#include<gsl/gsl_rng.h>
#include<gsl/gsl_randist.h>

#define INFO_OUT true
#define DEBUG_OUT false

// logger
Logger dbglog(stdout, "generator_dbg", DEBUG_OUT);
Logger genlog(stdout, "generator", INFO_OUT);

/** Just a circle at the origin. Params[0] is the radius. */
int circular(Array2d& in, const vector<double>& xs, const vector<double>& ys, const vector<double>& params) {
    int nx = xs.size(), ny = ys.size();
    double radius = params[0], r;

    for(int i = 0; i < nx; i ++ ) {
        for(int j = 0; j < ny; j ++ ) {
            r = xs[j] * xs[j] + ys[i] * ys[i];
            if(r <= radius*radius)
                in[i][j] = 1.0;
            else
                in[i][j] = 0.0;
        }
    }
    return 0;
}

/** A rectange of dimensions params[0] x params[1] */
int rectangle(Array2d& in, const vector<double>& xs, const vector<double>& ys, const vector<double>& params) {
    int nx = xs.size(), ny = ys.size();
    double ax = params[0], ay = params[1];

    for(int i = 0; i < nx; i ++ ) {
        for(int j = 0; j < ny; j ++ ) {
            if(abs(xs[j]) <= ax/2.0 && abs(ys[i]) <= ay/2.0)
                in[i][j] = 1.0;
            else
                in[i][j] = 0.0;
        }
    }
    return 0;
}

/** Gaussian illuminated circular aperture. params[0] is radius and params[1] is sigma */
int gaussian(Array2d& in, const vector<double>& xs, const vector<double>& ys, const vector<double>& params) {
    int nx = xs.size(), ny = ys.size();
    double R = params[0], sig = params[1];

    double R_sq = R * R;
    double sigsq2 = 2.0 * sig * sig;
    double rsq;

    for(int i = 0; i < nx; i ++ ) {
        for(int j = 0; j < ny; j ++ ) {
            rsq = xs[j] * xs[j] + ys[i] * ys[i];
            if(rsq <= R_sq)
                in[i][j] = exp(-rsq / sigsq2);
            else
                in[i][j] = 0.0;
        }
    }
    return 0;
}

/** A circular Gaussian aperture with a hole in the middle 
 * params[0] is the radius. params[1] is sigma. params[2] is the hole radius.
 */
int gaussian_hole(Array2d& in, const vector<double>& xs, const vector<double>& ys, const vector<double>& params) {
    int nx = xs.size(), ny = ys.size();
    double R_ext = params[0], sig = params[1], R_int = params[2];

    double R_ext_sq = R_ext * R_ext;
    double R_int_sq = R_int * R_int;
    double sigsq2 = 2.0 * sig * sig;
    double rsq;

    for(int i = 0; i < nx; i ++ ) {
        for(int j = 0; j < ny; j ++ ) {
            rsq = xs[j] * xs[j] + ys[i] * ys[i];
            if(rsq <= R_ext_sq && rsq >= R_int_sq)
                in[i][j] = exp(-rsq / sigsq2);
            else
                in[i][j] = 0.0;
        }
    }
    return 0;
}

/** A circular and holed aperture, with random errors on it.
 * params[0] is the outer radius. params[1] is sigma. params[2] is the inner(hole) radius.
 * params[3] is the sigma of the phase errors. params[4] is (optionally) the RNG seed.
 */
int rand_errors(Array2d& in, const vector<double>& xs, const vector<double>& ys, const vector<double>& params) {
    // this is just unpacking the arguments
    int nx = xs.size(), ny = ys.size();
    double R_ext_sq = params[0] * params[0];
    double sig_sq2 = 2 * params[1] * params[1];
    double R_int_sq = params[2] * params[2];
    double err_sigma = params[3];
    double rsq, phi;

    // initiate a random number generator for the errors
    gsl_rng * rng = gsl_rng_alloc(gsl_rng_default);
    if(params.size() > 4) {
        gsl_rng_set(rng, (unsigned long int)params[4]);
    }

    for(int i = 0; i < nx; i ++ ) {
        for(int j = 0; j < ny; j ++ ) {
            rsq = xs[j] * xs[j] + ys[i] * ys[i];
            if(rsq <= R_ext_sq && rsq >= R_int_sq) {
                phi = gsl_ran_gaussian(rng, err_sigma);
                in[i][j] = polar(exp(-rsq / sig_sq2), phi);
            }
            else
                in[i][j] = 0.0;
        }
    }

    gsl_rng_free(rng);
    return 0;
}


/** Helper: A gaussian mask to convolve with.
 * params[0] is the correlation length.
 * Void return so it can't be exposed via the names map at the bottom.
 */
void gauss_mask(Array2d &in, const vector<double>& xs, const vector<double>& ys, const vector<double>& params) {
    // unpack arguments
    int n_cols = xs.size(), n_rows = ys.size();
    double lc = params[0];          // the correlation length
    double sig_sq2 = 2 * lc * lc;   // the 2 sigma squared in the gaussian
    double rsq;

    // set the array values
    for(int i = 0; i < n_rows; i ++ )
        for(int j = 0; j < n_cols; j ++ ) {
            rsq = xs[j] * xs[j] + ys[i] * ys[i];
            in[i][j] = exp( - rsq / sig_sq2);
        }
}


/** Helper: round shape of radius params[0], filled with random real numbers.
 * The randomness is gaussian-distributed, with mean 0 and sigma given by params[1].
 * params[2] is the rng seed.
 * 
 * This is useful for convolving with the gaussian mask to produce correlated errors.
 * Also void return so it can't be included in the map.
 */
void real_errors(Array2d &in, const vector<double> &xs, const vector<double> &ys, const vector<double> &params) {
    // unpack the arguments
    int n_cols = xs.size(), n_rows = ys.size();

    double R_ext_sq = params[0] * params[0];
    double err_sigma = params[1];
    unsigned long seed = (unsigned long)params[2];

    double rsq;

    // initialise the rng with the given seed
    gsl_rng * rng = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(rng, seed);

    // put a random number at each point
    for(int i = 0; i < n_rows; i ++ )
        for(int j = 0; j < n_cols; j ++ ) {
            rsq = xs[j] * xs[j] + ys[i] * ys[i];
            if(rsq <= R_ext_sq)
                in[i][j] = gsl_ran_gaussian(rng, err_sigma);
        }
    gsl_rng_free(rng);
}


/** Correlated errors - bumps in the surface
 * This is achieved by taking random errors and convolving (multiplying in Fourier space)
 * with a gaussian. It's quite resource intensive.
 * WARNING: There's an assumption that nx, ny will always be the same, which should be true
 * as long as main isn't modified!
 * 
 * params are the same as above for 0 -- 4, and params[5] is the correlation length.
 */
int corr_errors(Array2d &in, const vector<double>& xs, const vector<double>& ys, const vector<double>& params) {
    // unpack the arguments
    int n_cols = xs.size(), n_rows = ys.size();

    double R_ext_sq = params[0] * params[0];
    double sig_sq2 = 2 * params[1] * params[1];
    double R_int_sq = params[2] * params[2];
    double err_sigma = params[3];
    double seed = params[4];
    double lc = params[5];

    double rsq, rho, phi;

    // declare the secondary array and fftw plans
    static thread_local Array2d sec_array(n_rows, n_cols);
    static thread_local fftw_plan fwd_plan, rev_plan, sec_plan;
    static thread_local bool init = false;

    // Planning: this only runs on the first call
    if(!init) {
        init = true;

        planner_mtx.lock();
        genlog("\t\tLocked. Planning convolution FFTs.");

        // to check behaviour in multithreading, print array addresses
        char buff[100];
        sprintf(buff, "In array ptr: %p", (void*)in.ptr());
        dbglog(buff);
        sprintf(buff, "Sec array ptr: %p", (void*)sec_array.ptr());
        dbglog(buff);

        fwd_plan = fftw_plan_dft_2d(n_rows, n_cols, in.ptr(), in.ptr(), FFTW_FORWARD, FFTW_MEASURE);
        rev_plan = fftw_plan_dft_2d(n_rows, n_cols, in.ptr(), in.ptr(), FFTW_BACKWARD, FFTW_MEASURE);
        sec_plan = fftw_plan_dft_2d(n_rows, n_cols, sec_array.ptr(), sec_array.ptr(), FFTW_FORWARD, FFTW_MEASURE);
        
        planner_mtx.unlock();
        genlog("\t\tUnlocked. Planning done.");
    }

    // init the arrays
    // note that real_errors writes real numbers to the array - the depth
    gauss_mask(sec_array, xs, ys, {lc});
    real_errors(in, xs, ys, {params[0] + 3*lc, err_sigma, seed});

    // execute forward ffts
    fftw_execute(fwd_plan);
    fftw_execute(sec_plan);

    // multiply and reverse FT, then shift to proper place
    in.mult(sec_array);
    fftw_execute(rev_plan);
    fftshift(in);

    // calculate the current RMS and normalize to get the desired RMS error
    double depth_sigma = mean_stddev(myre, in, xs, ys, params[0]).err;    
    in.mult_each(err_sigma / depth_sigma);

    // walk the array and set the depth as the phase,
    // and the amplitude as a gaussian taper
    for(int i = 0; i < n_rows; i ++ )
        for(int j = 0; j < n_cols; j ++ ) {
            rsq = xs[j] * xs[j] + ys[i] * ys[i];
            if(rsq <= R_ext_sq && rsq >= R_int_sq) {
                phi = real(in(i, j));
                rho = exp(- rsq / sig_sq2);
                in[i][j] = polar(rho, phi);
            }
            else
                in[i][j] = 0;
        }

    return 0;
}

map<string, aperture_generator> generators = {
    {"circular", circular},
    {"rectangle", rectangle},
    {"gaussian", gaussian},
    {"gaussian_hole", gaussian_hole},
    {"rand_errors", rand_errors},
    {"corr_errors", corr_errors}
};