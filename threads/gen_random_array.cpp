/*
 * compile with:
 * g++ gen_random_array.cpp -o gen_random_array
 * run with:
 * NUM_THREADS=4 ./gen_random_array 1000000
 */

#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct chunk {
    int *arr;
    int n;
    uint32_t seed;
};

void* gen_random_array(void *arg) {
    chunk *c = (chunk *)arg;
    mt19937 r(c->seed);
    uniform_int_distribution<int> dis(1, 1000);

    for (int i = 0; i < c->n; i++) {
        c->arr[i] = dis(r);
    }
}

int main(int argc, char *argv[]) {
    auto start = high_resolution_clock::now();
    
    long num_threads = 1, n = 100;
    const char* NUM_THREADS = getenv("NUM_THREADS");
    random_device dev;
	uniform_int_distribution<uint32_t> seed_dist(0,UINT_MAX);

    if (NUM_THREADS!=NULL) {
            num_threads = atoi(NUM_THREADS);
    }

    if (argc > 1) {
        n = atoi(argv[1]);
    }

    int *arr = new int[n];

    uint32_t seeds[num_threads];
    mt19937 r(dev());
    for (int i = 0; i < num_threads; i++) {
        seeds[i] = seed_dist(r);
    }

    pthread_t threads[num_threads];
    chunk chunks[num_threads];
    
    long chunk_size = n / num_threads;
    for (int i = 0; i < num_threads; i++) {
        auto c = &chunks[i];
        c->arr = arr + i * chunk_size;
        c->n = chunk_size;
        c->seed = seeds[i];
        pthread_create(&threads[i], NULL, gen_random_array, (void *)c);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    std::ofstream outFile("arr.txt");
    for (int i = 0; i < n; i++) {
        outFile << arr[i] << " ";
    }
    outFile.close();
    
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken: " << duration.count() << " milliseconds" << endl;

    delete[] arr;

    return 0;
}

