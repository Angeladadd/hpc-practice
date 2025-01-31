/*
 * compile with:
 * use gnu compiler collection version 14(default g++ on mac os is clang)
 * g++-14 -fopenmp sum_random_array.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lgmpxx -lgmp -o sum_random_array
 * run with:
 * OMP_NUM_THREADS=4 ./sum_random_array 1000000 ../threads/arr.txt
 */

#include <iostream>
#include <fstream>
#include <omp.h>
#include <gmpxx.h>
#include <chrono>

using namespace std;
using namespace std::chrono;


#pragma omp declare reduction(mpz_add : mpz_class : omp_out += omp_in) \
    initializer(omp_priv = mpz_class("0"))
int main(int argc, char *argv[]) {
    auto start = high_resolution_clock::now();

    long n = 100;
    string arrfile = "arr.txt";
    int num_threads = omp_get_max_threads();

    if (argc > 1) {
        n = atoi(argv[1]);
        arrfile = argv[2];
    }
    std::ifstream inFile(arrfile);
    int* arr = new int[n];
    for (int i = 0; i < n; i++) {
        inFile >> arr[i];
    }

    mpz_class sum("0");
    #pragma omp parallel for reduction(mpz_add:sum)
    for (int i = 0; i < n; i++) {
        sum += mpz_class(to_string(arr[i]));
    }

    gmp_printf("Total sum is: %Zd\n", sum.get_mpz_t());

    delete[] arr;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}