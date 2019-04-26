#include "array2d.h"

#include<gsl/gsl_rstat.h>

#define DEBUG_OUT false
Logger arr2dlog(stdout, "arr2d", DEBUG_OUT);

string lims_to_str(const Limits &l) {
    char buff[100];
    sprintf(buff, "%d\t%d\t%d\t%d", l[0], l[1], l[2], l[3]);
    return string(buff);
}

Array2d::Array2d(int size_x, int size_y) : nx(size_x), ny(size_y) {
    // log
    char msg[64];
    sprintf(msg, "constructed array %d x %d", nx, ny);
    arr2dlog(msg);

    // allocate memory
    arr = (complex<double>*) fftw_alloc_complex(nx * ny);
}

Array2d::~Array2d() {
    // log
    char msg[64];
    sprintf(msg, "destructed array %d x %d", nx, ny);
    arr2dlog(msg);

    // free memory
    fftw_free(ptr());
}

/** Return the value of element [ix][iy] through round bracket operator
 * Use like a(ix, iy). Return is immutable, good for use with const Array2d &.
 */
complex<double> Array2d::operator()(int ix, int iy) const {
    int idx = ny*ix + iy;
    return arr[idx];
}

/** Return a pointer to row number ix
 * Use like a[ix][iy] for value of element, like a normal 2d array.
 * Return is mutable.
 */
complex<double> * Array2d::operator[](int ix) {
    return (arr + (ny*ix));
}

/** Multiply the first array with the second element-wise,
 * storing results in the first.
 * Returns 0 if succesful or something else if failed.
 */
int Array2d::mult(const Array2d& a) {
    if(nx != a.nx) return -1;
    if(ny != a.ny) return -1;

    for(int i = 0; i < nx; i ++ )
        for(int j = 0; j < ny; j ++ )
            (*this)[i][j] *= a(i, j);
    return 0;
}

/** Multiply every element in the array by a scalar c */
int Array2d::mult_each(complex<double> c) {
    for(int i = 0; i < nx; i ++ )
        for(int j = 0; j < ny; j ++ )
            (*this)[i][j] *= c;
    return 0;
}

/** Divide every element in the array by a scalar c */
int Array2d::divide_each(complex<double> c) {
    return this->mult_each(1.0 / c);
}

/** Test for near equality to within EPS */
bool operator==(const Array2d &a, const Array2d &b) {
    if(a.nx != b.nx) return false;
    if(a.ny != b.ny) return false;
    
    int nx = a.nx, ny = a.ny;

    for(int i = 0; i < nx; i ++ )
        for(int j = 0; j < ny; j ++ ) {
            if( ! DBL_EQ(a(i, j), b(i, j)))
                return false;
        }
    return true;
}

/** Cast the pointer to fftw_complex*,
 * such that it can be used with fftw library
 */
fftw_complex * Array2d::ptr() {
    return (fftw_complex*) arr;
}


/** Find the x and y bounds within which the absolute value of property fun
 *  is greater than some fraction (rel_sens) of the maximum value
 *  OR just greater than abs_sens.
 * 
 *  To ignore one of absolute or relative sensitivities, set them to 0.
 */
Limits Array2d::find_interesting(complex_to_real fun, double abs_sens, double rel_sens) const {
    // check if caller wants to ignore one criterion
    if(abs_sens == 0.0) abs_sens = INFINITY; // nothing is greater than inf
    if(rel_sens == 0.0) rel_sens = 2.0;      // nothing is greater than 2*max

    int imin = nx, imax = 0, jmin = ny, jmax = 0;
    double fun_max = 0.0, abs_here;

    // walk the array once and find max abs value of fun
    for(int i = 0; i < nx; i ++ ) {
        for(int j = 0; j < ny; j ++ ) {
            abs_here = abs(fun((*this)(i, j)));
            if(abs_here > fun_max)
                fun_max = abs_here;
        }
    }

    // walk again and record where abs value of fun
    // is greater than fraction of maximum found earlier
    for(int i = 0; i < nx; i ++ ) {
        for(int j = 0; j < ny; j ++ ) {
            abs_here = abs(fun((*this)(i, j)));
            if(abs_here > rel_sens * fun_max || abs_here > abs_sens) {
                if(i > imax) imax = i;
                if(i < imin) imin = i;
                if(j > jmax) jmax = j;
                if(j < jmin) jmin = j;
            }
        }
    }
    // increase maxima by one to follow inclusive-exclusive convention
    imax++; jmax++;
    
    // log
    char msg[100];
    sprintf(msg, "\t\tLimits: rows %d -- %d ; cols %d -- %d", imin, imax, jmin, jmax);
    arr2dlog(msg);

    return Limits{imin, imax, jmin, jmax};
}


/** Print function fun applied to all the elements, formatted as 2d array
 */
void Array2d::print_prop(complex_to_real fun, FILE * out_file) const {
    for(int ix = 0; ix < nx; ix++) {
        for(int iy = 0; iy < ny; iy++)
            fprintf(out_file, PRINT_FORMAT, fun((*this)(ix, iy)));
        fprintf(out_file, "\n");
    }
}


/* Print function applied to all elements within limits specified in lim */
void Array2d::print_prop(complex_to_real fun, const Limits &lim, FILE * out_file) const {
    int imin = lim[0], imax = lim[1], jmin = lim[2], jmax = lim[3];

    for(int i = imin; i < imax; i++) {
        for(int j = jmin; j < jmax; j++)
            fprintf(out_file, PRINT_FORMAT, fun((*this)(i, j)));
        fprintf(out_file, "\n");
    }
}

/** Make a deep copy of this into another array, which must have the same size.
 * returns non-zero if copy failed.
 */
int Array2d::copy_into(Array2d &a) const {
    arr2dlog("copy_into called");
    if(a.nx != (*this).nx || a.ny != (*this).ny) return 1;

    for(int i = 0; i < nx; i ++ )
        for(int j = 0; j < ny; j ++ )
            a[i][j] = (*this)(i, j);
    
    return 0;
}

/** 
 * The zero-frequency component is shifted to the middle, IN PLACE!!!
 * The function allocates another array of the same size as a, so be careful
 * to not run out of memory.
 **/
void fftshift(Array2d &a) {
    arr2dlog("fftshift(Array2d) call");
    Array2d b(a.nx, a.ny);

    int shift_x = (a.ny+1) / 2;
    int shift_y = (a.nx+1) / 2;
    for(int i = 0; i < a.nx; i ++ )
        for(int j = 0; j < a.ny; j ++ ) {
            int source_i = (i + shift_y) % a.nx;
            int source_j = (j + shift_x) % a.ny;
            b[i][j] = a(source_i, source_j);
        }

    b.copy_into(a);
}

/** Find the first minimum of abs(fun) along the horizontal axis in the first row */
ValueError<double> find_first_min(complex_to_real fun, const Array2d &a, const vector<double> &xs) {
    int i = 0, j = 1;
    int nx = xs.size();

    // keep walking along the row until you find a local min
    while(j < nx-1)
        if(fun(a(i, j-1)) > fun(a(i, j)) && fun(a(i, j)) < fun(a(i, j+1)))
            break;
        else
            j++;

    ValueError<double> res;
    res.val = xs[j];
    res.err = abs(xs[j] - xs[j-1]);
    return res;
}

/**
 * Find the x-coordinate of the first half-power point and the error
 */
ValueError<double> hwhp(const Array2d &a, const vector<double> &xs) {
    int i = 0, j = 0;
    int nx = xs.size();

    double max_abs = abs(a(0, 0));
    double half_power = max_abs / sqrt(2.0);

    // walk along the first row until abs(a) drops below half_power
    while(j < nx-1)
        if(abs(a(i, j)) < half_power)
            break;
        else
            j ++;
    
    ValueError<double> res;
    res.val = xs[j];
    res.err = abs(xs[j] - xs[j-1]);
    return res;
}

/**
 * Calculate the mean and RMS of argument if abs is above a sensitivity
 */
ValueError<double> phase_rms(const Array2d &a, int n_rows, int n_cols) {
    const double eps = EPS;

    // running statistics initialization
    gsl_rstat_workspace * rstat = gsl_rstat_alloc();

    // walk the array, and add arg only if number is larger than eps
    for(int i = 0; i < n_rows; i ++ )
        for(int j = 0; j < n_cols; j ++ ) {
            if(abs(a(i, j)) > eps)
                gsl_rstat_add(arg(a(i, j)), rstat);
        }

    // cleanup and return
    ValueError<double> res;
    res.err = gsl_rstat_sd(rstat);
    res.val = gsl_rstat_mean(rstat);
    gsl_rstat_free(rstat);
    return res;
}


/**
 * Print the limits in two directions of the 2d array, then the array itself,
 * in standard formatted way. 
 * Only print stuff within x and y limits given by lims.
 */
void print_lim_array(FILE * filep, complex_to_real fun, const Array2d &a, const vector<double> &xs, const vector<double> &ys, const Limits &lims) {
    int imin = lims[0], imax = lims[1], jmin = lims[2], jmax = lims[3];

    fprintf(filep, "% 6.5f\t% 6.5f \n", xs[jmin], xs[jmax-1]);
    fprintf(filep, "% 6.5f\t% 6.5f \n", ys[imin], ys[imax-1]);
    a.print_prop(fun, lims, filep);
}
