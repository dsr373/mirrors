#include "util.h"

complex_to_real myabs = [](complex<double> z) -> double {return abs(z);};
complex_to_real myarg = [](complex<double> z) -> double {return arg(z);};
complex_to_real myre = [](complex<double> z) -> double {return real(z);};
complex_to_real complexness = [](complex<double> z) -> double {return abs(imag(z)/real(z));};

#define OPTION_ERROR "Incorrect option. Expected %s, got %s"

inline void option_error(const char * optname, const char * readname) {
    char msg[100];
    sprintf(msg, OPTION_ERROR, optname, readname);
    throw runtime_error(msg);
}

/** Overloaded utility function used to parse one line of the the config file
 * It expects a format like
 * `name = value`
 * where `name` has to match `optname` exactly. Only if that happens, `value` is
 * written to `option`.
 */
void read_option(FILE * filep, const char * optname, int &option) {
    char readname[64];
    int readval;
    fscanf(filep, " %s = %d ", readname, &readval);

    if(strcmp(readname, optname) != 0) option_error(optname, readname);
    option = readval;
}

void read_option(FILE * filep, const char * optname, double &option) {
    char readname[64];
    double readval;
    fscanf(filep, " %s = %lf ", readname, &readval);

    if(strcmp(readname, optname) != 0) option_error(optname, readname);
    option = readval;
}

void read_option(FILE * filep, const char * optname, string &option) {
    char readname[64];
    char readval[64];
    fscanf(filep, " %s = %s ", readname, readval);

    if(strcmp(readname, optname) != 0) option_error(optname, readname);
    option = string(readval);
}

void read_option(FILE * filep, const char * optname, vector<double> &option) {
    char readname[64], delim[2];
    double readval;
    fscanf(filep, " %s = ", readname);
    
    if(strcmp(readname, optname) != 0) option_error(optname, readname);

    while(fscanf(filep, " %lf", &readval) == 1) {
        option.push_back(readval);
        // consume terminating newline
        if(fscanf(filep, "%1[\n]", delim) == 1) break;
    }
}

void read_option(FILE * filep, const char * optname, vector<string> &option) {
    char readname[64], delim[2];
    char readval[64];
    fscanf(filep, " %s = ", readname);
    
    if(strcmp(readname, optname) != 0) option_error(optname, readname);

    while(fscanf(filep, " %s", readval) == 1) {
        option.push_back(string(readval));
        // consume the terminating newline
        if(fscanf(filep, "%1[\n]", delim) == 1) break;
    }
}

/** 
 * Parse configuration file `filename` and construct the Config object.
 */
Config::Config(const char * filename) {
    string cnf_filename = filename;

    int n_shapes = 0;
    // double f_tmp;

    FILE * cnf_filep = fopen(cnf_filename.c_str(), "r");
    if(cnf_filep == NULL) perror("Could not open config file.");
    
    read_option(cnf_filep, "nx", nx);
    read_option(cnf_filep, "ny", ny);
    read_option(cnf_filep, "prefix", out_prefix);
    read_option(cnf_filep, "tasks", tasks);
    read_option(cnf_filep, "n_shapes", n_shapes);

    for(int i = 0; i < n_shapes; i ++ ) {
        ShapeProperties sp;
        read_option(cnf_filep, "type", sp.generator_key);
        read_option(cnf_filep, "lx", sp.lx);
        read_option(cnf_filep, "ly", sp.ly);
        read_option(cnf_filep, "params", sp.shape_params);

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

void Logger::operator()(const string message) const {
    (*this).write(message.c_str());
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
