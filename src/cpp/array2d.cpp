#include "array2d.h"

#define DEBUG_OUT false

Logger arr2dlog(stdout, "arr2d", DEBUG_OUT);

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

/** Print function fun applied to all the elements, formatted as 2d array
 */
void Array2d::print_prop(complex_to_real fun, FILE * out_file) const {
    for(int ix = 0; ix < nx; ix++) {
        for(int iy = 0; iy < ny; iy++)
            fprintf(out_file, "% 6.5f\t", fun((*this)(ix, iy)));
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


/**
 * Print the limits in two directions of the 2d array, then the array itself,
 * in standard formatted way.
 */
void print_lim_array(FILE * filep, complex_to_real fun, const Array2d &a, const vector<double> &xs, const vector<double> &ys) {
    fprintf(filep, "% 6.5f\t% 6.5f \n", xs.front(), xs.back());
    fprintf(filep, "% 6.5f\t% 6.5f \n", ys.front(), ys.back());
    a.print_prop(fun, filep);
}

// ================ Aperture generators ================
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