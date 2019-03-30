#include "array2d.h"

Array2d::Array2d(int size_x, int size_y) : nx(size_x), ny(size_y) {
    arr = (complex<double>*) fftw_alloc_complex(nx * ny);
}

Array2d::~Array2d() {
    fftw_free(ptr());
}

/** Return the value of element [ix][iy] through round bracket operator
 * Use like a(ix, iy)
 */
complex<double> Array2d::operator()(int ix, int iy) {
    int idx = ny*ix + iy;
    return arr[idx];
}

/** Return a pointer to row number ix
 * Use like a[ix][iy] for value of element, like a normal 2d array
 */
complex<double> * Array2d::operator[](int ix) {
    return (arr + (ny*ix));
}

/** Cast the pointer to fftw_complex*,
 * such that it can be used with fftw library
 */
fftw_complex * Array2d::ptr() {
    return (fftw_complex*) arr;
}

/** Print function fun of the elements as a 2d array
 */
void Array2d::print_prop(complex_to_real fun, FILE * out_file) {
    for(int ix = 0; ix < nx; ix++) {
        for(int iy = 0; iy < ny; iy++)
            fprintf(out_file, "% 6.5f\t", fun((*this)[ix][iy]));
        fprintf(out_file, "\n");
    }
}


// Array2d fftshift(const Array2d &a) {
    
// }


vector<double> fftfreq(int n, double dt) {
    vector<double> v(n, 0.0);
    int halfpoint = (n+1)/2;

    for(int i = 0; i < halfpoint; i++)
        v[i] = i / (dt*n);

    for(int i = halfpoint; i < n; i ++)
        v[i] = i / (dt*n) - n;

    return v;
}