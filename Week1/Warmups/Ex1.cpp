#include <iostream>
#include <thread>

#define Loop(i,a,b) for (int i = a ; i < b ; i++)

using namespace std;

void welcome(int id, int* check){
    /*
        1. Try this code with both cout and printf
        2. Try uncommenting the second printf and observe the outputs
    */
    cout << "I am thread " << id << endl;
    printf("I am thread %d\n",id);
    printf("Bye %d\n",id);
    (*check)++;
    return;
}

int main(int argc, char* argv[]){
    int n;
    cin >> n;
    thread* t = new thread[n];
    int* id = new int[n];
    int check = 0;
    Loop(i,0,n){
        cout << "CURRENT VALUE OF CHECK IS " << check << endl;
        id[i] = i;
        int a = i;
        t[i] = thread(welcome,a, &check);
    }
    Loop(i,0,n) t[i].join();    
    delete [] id;
    delete [] t;
    return 0;
}