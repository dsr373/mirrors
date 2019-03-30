#include<cstdio>

#include "array2d.h"

void run_fftfreq(int n) {
    printf("run_fftfreq %d : \t", n);
    vector<double> f = fftfreq(n, 1.0/(double)n);

    for(int i = 0; i < n; i++)
        printf("% .1f\t", f[i]);
    printf("\n");
}

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

int main() {
    test_fftfreq();
    test_fftshift();
    return 0;
}