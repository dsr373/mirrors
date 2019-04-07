#include "array2d.h"

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

/** Test for near equality to within EPS */
bool operator==(const Array2d &a, const Array2d &b) {
    if(a.nx != b.nx) return false;
    if(a.ny != b.ny) return false;
    
    int nx = a.nx, ny = a.ny;

    for(int i = 0; i < nx; i ++ )
        for(int j = 0; j < ny; j ++ ) {
            if( ! DBL_EQ(abs(a(i, j) - b(i, j)), 0.0))
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


/** Deep copy the array. Written to avoid deep copying on copy-constructor
 * or equality assignment,
 * which would be quite expensive on large arrays given O(n^2).
 */
Array2d Array2d::deep_copy() const {
    arr2dlog("explicit deep copy call");
    Array2d aa(nx, ny);

    for(int i = 0; i < nx; i ++ ) {
        for(int j = 0; j < ny; j ++ ) {
            aa[i][j] = (*this)(i, j);
        }
    }

    return aa;
}

/** Create a transposed copy of the array */
Array2d Array2d::transpose() const {
    arr2dlog("explicit transpose call");
    Array2d a(ny, nx);

    for(int i = 0; i < nx; i ++ )
        for(int j = 0; j < ny; j ++ )
            a[j][i] = (*this)(i, j);
    return a;
}

/** 
 * Create new Array2d with the zero-frequency component is shifted to the middle.
 * Equivalent to applying an operation like fftshift on vectors
 * to both dimensions of the array 
 **/
Array2d fftshift(const Array2d &a) {
    arr2dlog("Array2d fftshift(Array2d) call");
    Array2d b(a.nx, a.ny);

    // first shift in x
    int shift_x = (a.ny+1) / 2;
    int shift_y = (a.nx+1) / 2;
    for(int i = 0; i < a.nx; i ++ )
        for(int j = 0; j < a.ny; j ++ ) {
            int source_i = (i + shift_y) % a.nx;
            int source_j = (j + shift_x) % a.ny;
            b[i][j] = a(source_i, source_j);
        }
    return b;
}


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

// ================ Aperture generators ================

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

map<string, aperture_generator> generators = {
    {"circular", circular},
    {"rectangle", rectangle},
    {"gaussian", gaussian},
    {"gaussian_hole", gaussian_hole}
};