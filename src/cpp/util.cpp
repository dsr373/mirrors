#include "util.h"

Config::~Config() {
    fclose(in_filep);
    fclose(out_filep);
}

/** 
 * Parse command line options and write them into the config structure.
 */
int configure(int argc, char * argv[], Config &c) {
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
    c.in_filep = fopen(ifile, "w");
    c.out_filep = fopen(ofile, "w");

    return 0;
}