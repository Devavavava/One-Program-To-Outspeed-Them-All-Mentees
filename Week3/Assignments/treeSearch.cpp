#include <iostream>
#include <thread>
#include <xmmintrin.h>
#include <chrono>
#include <bits/stdc++.h>

using namespace std;

/*

TASK:
Given a tree consisting of nodes and a special integer k, find the secret of the node with value k
A secret of a node t is defined as a string s, such that s[i] is the bit of the ith node occurring on the path from root to t 
Each node contains 3 members : 
1] value - the integer value of the node (unique to each node)
2] bit - a character of each node (not necessarily unique)
3] children - a vector containing pointers to all children of the node
YOU MUST OUTPERFORM THE NAIVE IMPLEMENTATION

*/

class node {
public:
    int value;
    vector<node*> children;
    char bit;
    node* parent;
};

class tree {
private:
    int n;
    vector<node *> listOfNodes;
public:
    node* root;
    void addConnection (node *a, node *b) { // adds b as child of a
        a->children.push_back(b);
    }
    tree(int a) {
        n = a;
    }
    void setListOfNodes (vector<node*> list) {
        if (list.size() != n) {
            cout<<"ERROR, PROVIDED LIST SIZE DOES NOT MATCH SIZE OF TREE!\n";
            exit(1);
        }
        listOfNodes = list;
    }
    ~tree() {
        for (int i=0; i<n; i++) {
            delete listOfNodes[i];
        }
    }
    int getSize () {
        return n;
    }
};

string naiveSearch (tree &T, int k) {
    queue<node*> q;
    q.push(T.root);
    while (!q.empty()) {
        node *tmp = q.front();
        q.pop();
        if (tmp->value == k) { // found it!
            vector<node*> seq; // vector to construct the string
            seq.push_back(tmp);
            node *u = tmp->parent;
            while (u != NULL) {
                seq.push_back(u);
                u = u->parent;    
            }
            string t;
            for (int i=seq.size() - 1; i >= 0; i--) {
                t += seq[i]->bit;
            }
            return t;
        }
        for (auto x : tmp->children) {
            q.push(x);
        }
    }
    return "\0";
}

string devsSearch(node* startNode, int k) {
    node* tmp = startNode;
    if (tmp->value == k) { // found it!
        vector<node*> seq; // vector to construct the string
        seq.push_back(tmp);
        node* u = tmp->parent;
        while (u != NULL) {
            seq.push_back(u);
            u = u->parent;
        }
        string t;
        for (int i = seq.size() - 1; i >= 0; i--) {
            t += seq[i]->bit;
        }
        return t;
    }
    for (auto x : tmp->children) {
        string result = devsSearch(x, k);
        if (!result.empty()) {
            return result;
        }
    }
    return "\0";
}

void threadFunc(node* startNode, int k, vector<string>& results, mutex& resultsMutex) {
    string result = devsSearch (startNode, k);
    if (!result.empty()) {
        lock_guard<mutex> lock(resultsMutex);
        results.push_back(result);
    }
}

string optim(tree &T, int k) {
    queue<node*> q;
    q.push(T.root);
    while(q.size() < 15){
        if (q.size() + q.front()->children.size() <= 16) {
            for (auto x : q.front()->children) {
                q.push(x);
            }
            q.pop();
        } 
        else {
            break;
        }
    }

    vector<node*> nodes;
    while (!q.empty()) {
        nodes.push_back(q.front());
        q.pop();
    }

    cout << "Number of nodes: " << nodes.size() << endl;
    
    vector<std::thread> threads;
    vector<string> results;
    mutex resultsMutex;

    for (auto* node : nodes) {
        threads.emplace_back(threadFunc, node, k, std::ref(results), std::ref(resultsMutex));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Process results
    // For simplicity, just return the first non-empty result
    for (const auto& result : results) {
        if (!result.empty()) {
            return result;
        }
    }

    return "\0";
}

int main() {
    int n;
    // cin >> n;
    n = 10000;
    tree T(n);
    vector<node*> list;
    node* u = new node;
    u->value = 0;
    u->bit = 'a';
    u->parent = NULL;
    T.root = u;
    list.push_back(u);
    char currChar = 'b';
    for (int i = 1; i < n; i++) {
        node *tmp = new node;
        tmp->value = i;
        tmp->bit = currChar;
        if (currChar == 'z') {
            currChar = 'a';
        } else {
            currChar++;
        }
        if (i % 3 == 0) {
            T.addConnection(list[i / 3 - 1], tmp);
            tmp->parent = list[i/3-1];
        } else {
            T.addConnection(list[i / 3], tmp);
            tmp->parent = list[i/3];
        }
        list.push_back(tmp);
    }
    T.setListOfNodes(list);

    auto startNaive = chrono::high_resolution_clock::now();
    string naive = naiveSearch(T,n-1);
    auto endNaive = chrono::high_resolution_clock::now();
    auto naiveTime = chrono::duration_cast<chrono::duration<double>>(endNaive - startNaive);
    cout<<"Naive string : "<<naive<<endl;
    cout<<"Naive time : "<<naiveTime.count()<<endl;

    auto startOptim = chrono::high_resolution_clock::now();
    string optimSearch = optim(T, n - 1);
    auto endOptim = chrono::high_resolution_clock::now();
    auto optimTime = chrono::duration_cast<chrono::duration<double>>(endOptim - startOptim);

    cout<<"Optim string : "<<optimSearch<<endl;
    cout<<"Optim time : "<<optimTime.count()<<endl;

    cout<<"Speedup : "<<naiveTime.count()/optimTime.count()<<endl;
    return 0;
}
