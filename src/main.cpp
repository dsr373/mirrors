#include<cstdio>
#include<cmath>
#include<complex>
#include<cstring>

#include<gsl/gsl_sf_trig.h>
#include<fftw3.h>

#include "array2d.h"

using namespace std;

#define N 50

int main(int argc, char * argv[]) {
    // parse command line args
    // either none or two arguments must be provided, giving the input and output filenames
    char ifile[] = "data/in.txt";
    char ofile[] = "data/out.txt";
    if(argc > 1) {
        if(argc == 3) {
            strcpy(ifile, argv[1]);
            strcpy(ofile, argv[2]);
        }
        else {
            printf("Error: incorrect number of arguments!\n");
            return 1;
        }
    }

    // open files
    FILE * in_file = fopen(ifile, "w");
    FILE * out_file = fopen(ofile, "w");

    // declarations
    double wx = 0.628;    // 0.2 pi
    double wy = 0.314;    // 0.1 pi

    Array2d in(N, N);
    Array2d out(N, N);
    fftw_plan p;

    // allocate
    p = fftw_plan_dft_2d(N, N, in.ptr(), out.ptr(), FFTW_FORWARD, FFTW_ESTIMATE);

    for(int i = 0; i < N; i++) 
        for(int j = 0; j < N; j++) {
            double re = gsl_sf_cos(wx*i + wy*j);
            double im = gsl_sf_sin(wx*i + wy*j);
            in[i][j] = complex<double>(re, im);
    }

    // plan
    fftw_execute(p);
    // print arrays
    complex_to_real myabs = [](complex<double> z) -> double {return abs(z);};
    in.print_prop(myabs, in_file);
    out.print_prop(myabs, out_file);

    // clean up and exit
    fftw_destroy_plan(p);
    fclose(in_file); fclose(out_file);
    return 0;
}
