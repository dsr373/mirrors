#include "util.h"

complex_to_real myabs = [](complex<double> z) -> double {return abs(z);};
complex_to_real myarg = [](complex<double> z) -> double {return arg(z);};
complex_to_real myre = [](complex<double> z) -> double {return real(z);};

/** Overloaded utility function used to parse one line of the the config file
 * It expects a format like
 * ```name = value```
 * where `name` has to match `optname` exactly. Only if that happens, `value` is
 * written to `option`.
 */
void read_option(FILE * filep, const char * optname, int &option) {
    char readname[64];
    int readval;
    fscanf(filep, " %s = %d ", readname, &readval);

    if(strcmp(readname, optname) == 0) option = readval;
    else {
        char msg[100];
        sprintf(msg, "Incorrect option. Expected %s, got %s", optname, readname);
        throw runtime_error(msg);
    }
}

void read_option(FILE * filep, const char * optname, double &option) {
    char readname[64];
    double readval;
    fscanf(filep, " %s = %lf ", readname, &readval);

    if(strcmp(readname, optname) == 0) option = readval;
    else {
        char msg[100];
        sprintf(msg, "Incorrect option. Expected %s, got %s", optname, readname);
        throw runtime_error(msg);
    }
}

void read_option(FILE * filep, const char * optname, string &option) {
    char readname[64];
    char readval[64];
    fscanf(filep, " %s = %s ", readname, readval);

    if(strcmp(readname, optname) == 0) option = string(readval);
    else {
        char msg[100];
        sprintf(msg, "Incorrect option. Expected %s, got %s", optname, readname);
        throw runtime_error(msg);
    }
}

/** 
 * Parse command line options and write them into the config structure.
 * The program can be run in three ways:
 * 1. no options. Then the defaults are used.
 * 2. One option. Only the config file is given: `main.exe filename`
 * 3. Three options. The config file and prefixes for results files:
 *      `main.exe filename in_prefix out_prefix`
 */
Config::Config(int argc, char * argv[]) {
    // defaults
    in_prefix = "data/in";
    out_prefix = "data/out";
    string cnf_filename = "config/config.txt";

    int n_shapes = 0;
    double f_tmp;

    if(argc == 2) {
        cnf_filename = string(argv[1]);
    } else if(argc == 4) {
        cnf_filename = string(argv[1]);
        in_prefix = string(argv[2]);
        out_prefix = string(argv[3]);
    } else if(argc != 1)
        throw runtime_error("Incorrect number or arguments. "
        "Usage: main.exe [config_filename] [in_prefix out_prefix]");
    
    FILE * cnf_filep = fopen(cnf_filename.c_str(), "r");
    if(cnf_filep == NULL)
        throw runtime_error("Could not open config file.");
    
    read_option(cnf_filep, "nx", nx);
    read_option(cnf_filep, "ny", ny);
    read_option(cnf_filep, "n_shapes", n_shapes);

    for(int i = 0; i < n_shapes; i ++ ) {
        ShapeProperties sp;
        read_option(cnf_filep, "type", sp.generator_key);
        read_option(cnf_filep, "lx", sp.lx);
        read_option(cnf_filep, "ly", sp.ly);
        while(fscanf(cnf_filep, "%lf ", &f_tmp) == 1) {
            sp.shape_params.push_back(f_tmp);
        }

        shapes.push_back(sp);
    }
}

/** Construct logger that writes to file pointer filep,
 *  prepending name given, only if enabled == true
 */
Logger::Logger(FILE * filep, const char * name, bool enabled) : 
    filep(filep),
    name(name),
    enabled(enabled) {}

/* Write logger name + given message to file pointer, only if enabled */
void Logger::write(const char * message) const {
    if(enabled) {
        fprintf(filep, "%s: %s\n", name.c_str(), message);
    }
}

/* Same as calling write */
void Logger::operator()(const char * message) const {
    (*this).write(message);
}


vector<double> fftfreq(int n, double dt) {
    vector<double> v(n, 0.0);
    int halfpoint = (n+1)/2;

    for(int i = 0; i < halfpoint; i++)
        v[i] = i / (dt*n);

    for(int i = halfpoint; i < n; i ++)
        v[i] = (i - n) / (dt*n);

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
