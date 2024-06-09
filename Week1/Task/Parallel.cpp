#include "matrix.h"
#define Loop(i,a,b) for (int i = a ; i < b ; i++)
#define MAX_THREADS 8
using namespace std;

#include <thread>

Matrix::Matrix(int a, int b) { // generate a matrix (2D array) of dimensions a,b
    this->n = a;
    this->m = b;
    this->M = new int*[a];
    Loop(i, 0, n) this->M[i] = new int[b];
    this->initialiseMatrix();
}

Matrix::~Matrix() { // cleanup heap memory
    Loop(i, 0, this->n) delete[] this->M[i];
    delete[] this->M;
}

void Matrix::initialiseMatrix(){ // initialise entries to 0
    Loop(i, 0, this->n) {
        Loop(j, 0, this->m) this->M[i][j] = 0;
    }
}

void Matrix::inputMatrix() { // take input
    Loop(i, 0, this->n) {
        Loop(j, 0, this->m) cin >> this->M[i][j];
    }
}

void Matrix::displayMatrix() { // print matrix
    Loop(i, 0, this->n) {
        Loop(j, 0, this->m) cout << this->M[i][j] << " ";
        cout << "\n";
    }
}
int** Matrix::T(){
    int** MT = new int*[this->m];
    Loop(i,0,m) MT[i] = new int[this->n];
    Loop(i,0,m){
        Loop(j,0,n){
            MT[i][j] = this->M[j][i];
        }
    }
    return MT;
}

void Matrix::computeBlock(int start, int end, Matrix* t, Matrix* n, Matrix* c) {
    // Somehow this is slower when the loops are replaced with do-whiles 
    Loop (i, start, end)
        Loop (j, 0, c->m)
            Loop (k, 0, t->m)
                c->M[i][j] += t->M[i][k] * n->M[k][j];
}

Matrix* Matrix::multiplyMatrix(Matrix* N) {
    if (this->m != N->n) {
        return NULL;
    }
    Matrix *c = new Matrix(this->n,N->m);

    int total = c->n;
    int part = total / MAX_THREADS;
    int remainder = total % MAX_THREADS;

    std::thread threads[MAX_THREADS]; 

    for (int i = 0; i < MAX_THREADS; ++i) {
        threads[i] = std::thread([=]() {
            int start = i * part + std::min(i, remainder);
            int end = start + part + (i < remainder);
            computeBlock(start, end, this, N, c);
        });
    }

    for (int i = 0; i < 8; ++i) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }
    return c;
}

