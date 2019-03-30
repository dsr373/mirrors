#include<cstdio>

#include "array2d.h"

// returns the abs of a complex. Used later for printing
complex_to_real myabs = [](complex<double> z) -> double {return abs(z);};

/** Run fftfreq with n integer results expected, and print returns */
void run_fftfreq(int n) {
    printf("run_fftfreq %d : \t", n);
    vector<double> f = fftfreq(n, 1.0/(double)n);

    for(int i = 0; i < n; i++)
        printf("% .1f\t", f[i]);
    printf("\n");
}

/** Test if fftfreq gives correct results */
void test_fftfreq() {
    const double eps = 1e-6;
    vector<int> ns = {4, 5};
    
    vector<double> expv, v;

    for(vector<int>::iterator it = ns.begin(); it != ns.end(); it++) {
        int n = *it;
        printf("test_fftfreq %d : ", n);

        v = fftfreq(n, 1.0/(double)n);
        if(n == 4)
            expv = {0.0, 1.0, -2.0, -1.0};
        else if(n == 5)
            expv = {0.0, 1.0, 2.0, -2.0, -1.0};

        for(int i = 0; i < n; i++)
        if(abs(v[i] - expv[i]) > eps) {
            printf("Failed: i = %d expected % .1f got % .1f\n", i, expv[i], v[i]);
            return;
        }
        printf("OK\n");
    }
}

void run_fftshift(int n) {
    printf("run_fftshift %d :", n);
    vector<double> v = fftfreq(n, 1.0/(double)n);
    vector<double> shifted = fftshift(v);

    printf("\nbefore:\t");
    for(int i = 0; i < n; i++)
        printf("% .1f\t", v[i]);
    
    printf("\nafter :\t");
    for(int i = 0; i < n; i++)
        printf("% .1f\t", shifted[i]);
    
    printf("\n");
}

void test_fftshift() {
    const double eps = 1e-6;
    vector<int> ns = {4, 5};
    
    vector<double> expv, v;

    for(vector<int>::iterator it = ns.begin(); it != ns.end(); it++) {
        int n = *it;
        printf("test_fftshift %d : ", n);

        v = fftshift(fftfreq(n, 1.0/(double)n));
        if(n == 4)
            expv = {-2, -1, 0, 1};
        else if(n == 5)
            expv = {-2, -1, 0, 1, 2};

        for(int i = 0; i < n; i++)
        if(abs(v[i] - expv[i]) > eps) {
            printf("Failed: i = %d expected % .1f got % .1f\n", i, expv[i], v[i]);
            return;
        }
        printf("OK\n");
    }
}

void test_array2d_equality() {
    printf("test_array2d_equality : ");

    Array2d a(2, 1);
    a[0][0] = 0; a[0][1] = 2;

    // very slightly different
    Array2d b(2, 1);
    b[0][0] = 0.0; b[0][1] = 2.0 + 1e-8;
    
    // very different
    Array2d c(2, 1);
    c[0][0] = 0.1; c[0][1] = 2;

    if(!(a == b)) {
        printf("FAILED on a == b\n");
        return;
    }
    if(a == c) {
        printf("FAILED on a == c\n");
        return;
    }
    printf("OK\n");
}

void test_array2d_deepcopy() {
    printf("test_array2d_deepcopy : ");

    // construct one matrix
    Array2d a(3, 2);
    a[0][0] = 0; a[0][1] = 1; a[0][2] = 2;
    a[1][0] = 3; a[1][1] = 4; a[1][2] = 5;

    // test if deep copy does actually copy well
    Array2d aa = a.deep_copy();
    if(!(aa == a)) {
        printf("FAILED at deep copy check.\n");
        return;
    }

    // test if true deep copy, i.e. changes to one don't change to other
    aa[0][1] = 6;
    if(aa == a) {
        printf("FAILED at modification check.\n");
        return;
    }
    printf("OK\n");
}

int main() {
    test_fftfreq();
    test_fftshift();
    test_array2d_equality();
    test_array2d_deepcopy();
    return 0;
}