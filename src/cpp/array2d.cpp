#include "array2d.h"

Array2d::Array2d(int size_x, int size_y) : nx(size_x), ny(size_y) {
    arr = (complex<double>*) fftw_alloc_complex(nx * ny);
}

Array2d::~Array2d() {
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
    fprintf(filep, "% 6.5f\t% 6.5f \n", *xs.begin(), *xs.end());
    fprintf(filep, "% 6.5f\t% 6.5f \n", *ys.begin(), *ys.end());
    a.print_prop(fun, filep);
}