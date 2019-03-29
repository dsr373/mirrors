#ifndef MYUTIL
#define MYUTIL

#include<cstdio>
#include<cstring>

/**
 * Struct containing the configuration of the program.
 * in_filep is a file pointer for the input array;
 * out_filep is a file pointer for the output array;
 */
struct Config {
    FILE * in_filep;
    FILE * out_filep;
    ~Config();
};

/** 
 * Parse command line options and write them into the config structure.
 */
int configure(int argc, char * argv[], Config &c);

#endif