#include <iostream>
#include <thread>
#include <immintrin.h>
#include <chrono>
#include <iomanip>
#include <immintrin.h> // SIMD
#include <cstring>
#include <mutex>
#include <condition_variable>

class Matrix {
    private:
        int** M;
        int n, m;

    public:
        Matrix(int a, int b);
        ~Matrix();
        void initialiseMatrix();
        void inputMatrix();
        void displayMatrix();

        void computeBlock(int start, int end, Matrix* A, Matrix* B, Matrix* C);

        int** T();
        Matrix* multiplyMatrix(Matrix* N);

        // helper function
        void set (int i, int j, int num) {
            M[i][j] = num;
        }

};