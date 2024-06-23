#include <iostream>
#include <thread>
#include <immintrin.h>
#include <chrono>
#include <bits/stdc++.h>
using namespace std;

#define MAX 10000000
#define MIN 0.000001

/*

TASK : Given an array of floats A, return an array of probabilities B, such that B[i] = softmax(1/sqrt(A[i]))
The softmax function is defined is follows:
softmax(A)[i] = A[i]/sum_{j=0}^{j=sizeof(A)}(A[j])
The "naive" way of doing this is implemented for you

*/

float quake3Algo (float number) {

    // check out https://en.wikipedia.org/wiki/Fast_inverse_square_root

    union {
        float f;
        uint32_t i;
    } conv;

    float x2;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    conv.f  = number;
    conv.i  = 0x5f3759df - ( conv.i >> 1 ); // evil floating point bit level hacking
    conv.f  = conv.f * ( threehalfs - ( x2 * conv.f * conv.f ) );
    return conv.f;
}

void naive (float *A, float *B, int n) {
/*

INPUT : float *A, int n
OUTPUT : float *B = softmax(1/sqrt(A))
To be honest this algorithm is not quite naive, it uses the quake 3 algorithm to compute fast inverse square root in O(nlogn)
Then it computes softmax in O(n)

*/

    for (int i=0; i<n; i++) {
        B[i] = quake3Algo(A[i]); // yes u could just use math.sqrt instead but it would not be nearly this cool
    }
    float sum = 0;
    for (int i=0; i<n; i++) {
        B[i] = exp(-B[i]);
    }

    for (int i=0; i<n; i++) {
        sum += B[i];
    }

    for (int i=0; i<n; i++) {
        B[i] = B[i]/sum;
    }

}

void processChunk(float* A, float* B, int start, int end, float& sum) {
    __m128 store128;
    float localSum = 0;
    for (int i = start; i < end; i += 4) {
        // Load 4 values at once
        store128 = _mm_loadu_ps(A + i);
        // Compute reciprocal square root for 4 values at once
        store128 = _mm_rsqrt_ps(store128);
        // Store the result
        _mm_storeu_ps(B + i, store128);
        // SIMD version of exp and sum calculation could go here
        // For simplicity, using scalar version as a placeholder
        for (int j = 0; j < 4 && (i + j) < end; ++j) {
            B[i + j] = exp(-B[i + j]);
            localSum += B[i + j];
        }
    }
    sum += localSum; // Accumulate the sum (needs synchronization)
}


void optimal (float *A, float *B, int n) {
    float sum = 0;
    // const int numThreads = std::thread::hardware_concurrency();
    const int numThreads = 8;
    std::vector<std::thread> threads;
    std::vector<float> partialSums(numThreads, 0);
    // cout << "Number of threads: " << numThreads << endl;

    int chunkSize = n / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? n : start + chunkSize;
        threads.emplace_back(processChunk, A, B, start, end, std::ref(partialSums[i]));
    }

    for (auto& t : threads) {
        t.join();
    }

    for (auto ps : partialSums) {
        sum += ps;
    }

    // Normalize B
    __m128 sumVec = _mm_set1_ps(sum);
    for (int i = 0; i < n; i += 4) {
        __m128 bVec = _mm_loadu_ps(B + i);
        bVec = _mm_div_ps(bVec, sumVec);
        _mm_storeu_ps(B + i, bVec);
    }
}
float A[100000000];
float BNaive[100000000];
float BOptim[100000000];

int main () {

    int n;
    // cin>>n;
    n = 200000;
    for (int i=0; i<100000000; i++) {
        if (i%100 != 0) {
            A[i] = MIN;
        }
        else {
            A[i] = MAX;
        }
    }

    auto startNaive = chrono::high_resolution_clock::now();
    naive(A,BNaive,100000000);
    auto endNaive = chrono::high_resolution_clock::now();
    auto naiveTime = chrono::duration_cast<chrono::duration<double>>(endNaive - startNaive);

    auto startOptim = chrono::high_resolution_clock::now();
    for (int i=0; i<10; i++)
        optimal(A,BOptim,100000000);
    auto endOptim = chrono::high_resolution_clock::now();
    auto optimTime = chrono::duration_cast<chrono::duration<double>>(endOptim - startOptim);

    for (int i=0; i<n; i++) {
        if (abs(BNaive[i]-BOptim[i]) > 0.0001) {
            cout<<"Naive and Optim answers do not match"<<endl;
        }
    }

    cout<<endl;
    cout<<"Naive time : "<<naiveTime.count()<<endl;
    cout<<"Optim time : "<<optimTime.count() / 10<<endl;

}