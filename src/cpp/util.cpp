#include "util.h"

complex_to_real myabs = [](complex<double> z) -> double {return abs(z);};
complex_to_real myarg = [](complex<double> z) -> double {return arg(z);};
complex_to_real myre = [](complex<double> z) -> double {return real(z);};

Config::~Config() {
    fclose(in_filep);
    fclose(out_filep);
}

/** 
 * Parse command line options and write them into the config structure.
 */
Config::Config(int argc, char * argv[]) {
    char ifile[] = "data/in.txt";
    char ofile[] = "data/out.txt";
    if(argc == 3) {
        strcpy(ifile, argv[1]);
        strcpy(ofile, argv[2]);
    }
    else if(argc != 1) {
        fprintf(stderr, "Incorrect number of arguments. Usage:\n"
        "main.exe [config_file] [infile outfile]\n");
        throw runtime_error("incorrect number of arguments");
    }

    // open files
    in_filep = fopen(ifile, "w");
    out_filep = fopen(ofile, "w");

    if(in_filep == NULL || out_filep == NULL) {
        fprintf(stderr, "One of the files couldn't be opened.\n");
        throw runtime_error("failed to open file");
    }
}


vector<double> fftfreq(int n, double dt) {
    vector<double> v(n, 0.0);
    int halfpoint = (n+1)/2;

    for(int i = 0; i < halfpoint; i++)
        v[i] = i / (dt*n);

    for(int i = halfpoint; i < n; i ++)
        v[i] = i / (dt*n) - n;

    return v;
}


/**
 * Calculate the coordinates of n evenly distributed points between -l/2 and l/2
 */
vector<double> coords(double l, int n) {
    vector<double> x(n);
    for(int i = 0; i < n; i ++ )
        x[i] = (i - n/2) * l/n;
    
    return x;
}
