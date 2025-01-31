/*
 * run with:
* g++ sum_random_array.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lgmpxx -lgmp -o sum_random_array
*/

#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include <chrono>
#include <string>
#include <gmpxx.h>

using namespace std;
using namespace std::chrono;

struct chunk {
    int *arr;
    int n;
};

void* sum_array(void *arg) {
    chunk *c = (chunk *)arg;
    mpz_class sum("0");
    for (int i = 0; i < c->n; i++) {
        sum = sum + mpz_class(to_string(c->arr[i]));
    }
    return (void *)new mpz_class(sum);
}

int main(int argc, char *argv[]) {
    auto start = high_resolution_clock::now();

    long num_threads = 1, n = 100;
    string arrfile = "arr.txt";
    const char* NUM_THREADS = getenv("NUM_THREADS");
    if (NUM_THREADS!=NULL) {
        num_threads = atoi(NUM_THREADS);
    }

    if (argc > 1) {
        n = atoi(argv[1]);
        arrfile = argv[2];
    }
    std::ifstream inFile(arrfile);
    int* arr = new int[n];
    for (int i = 0; i < n; i++) {
        inFile >> arr[i];
    }

    long chunk_size = n / num_threads;
    pthread_t threads[num_threads];
    chunk chunks[num_threads];

    for (int i = 0; i < num_threads; i++) {
        auto c = &chunks[i];
        c->arr = arr + i * chunk_size;
        c->n = chunk_size;
        pthread_create(&threads[i], NULL, sum_array, (void *)c);
    }

    mpz_class total_sum("0");
    for (int i = 0; i < num_threads; i++) {
        void *sum;
        pthread_join(threads[i], &sum);
        total_sum = total_sum + *(mpz_class *)sum;
        cout << "Thread " << i << " sum: " << *(mpz_class *)sum << endl;
    }

    cout << "Total sum: " << total_sum << endl;

    delete[] arr;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}