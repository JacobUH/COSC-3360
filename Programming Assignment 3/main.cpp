// Programming Assignment 3 (main.cpp)

#include <fstream>
#include <iostream>
#include <sstream>
#include "huffmanTree.h"
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <pthread.h>
using namespace std;

// Decompressed Node
struct Decompress {
    Node *top;
    string code;
    vector <int> positions;
    char *beginning;
    char symbol;
    int threadNumber;
    int *turn;
    pthread_mutex_t *bsem;
    pthread_mutex_t *bsem2;
    pthread_cond_t *waitTurn;
};

void *decompressor(void *arg){
    struct Decompress *argument = (Decompress*) arg;
    string binaryCode = argument->code;
    int threadNumber = argument->threadNumber;
    vector <int> positions = argument->positions;
    
    pthread_mutex_unlock(argument->bsem);
    pthread_mutex_lock(argument->bsem2);
    // wait if not correct turn
    while (threadNumber != (*argument->turn)){
        pthread_cond_wait(argument->waitTurn, argument->bsem2);
    }
    pthread_mutex_unlock(argument->bsem2);
    
    // do operation here
    Node *cur = argument->top;
    for(int i = 0; i < binaryCode.size(); i++){
        if (binaryCode[i] == '0'){
          cur = cur->left;
        }
        else{
          cur = cur->right;
        }
    }
    cout << "Symbol: " << cur->character << ", Frequency: " <<  cur->frequency << ", Code: " << binaryCode << endl;
    // argument->symbol = cur->character;

    // Storing the symbols in the positions vector
    for (int i = 0; i < positions.size(); i++){
        int position = positions[i];
        *(argument->beginning + position) = cur->character;
    }
    
    pthread_mutex_lock(argument->bsem2);
    (*argument->turn)++;
    
    // free waiting threads
    pthread_mutex_unlock(argument->bsem2);
    pthread_cond_broadcast(argument->waitTurn);
    return nullptr;
}

int main(int argc, char *argv[]){
    //Comment out when working 
    //ifstream input("input2.txt");

    // Input redirection and put characters and frequencies into vectors
    string loops;
    string inStr;
    vector <char> chars;
    vector <int> freqs;

    getline(cin, loops);
    //getline(input, loops);
    int size = stoi(loops);

    for (int i = 0; i < size; i++){
        getline(cin,inStr);
        //getline(input, inStr);
        char chara;
        int freq;
        istringstream iss(inStr);
        if (inStr.at(0) == ' '){
        chars.push_back(' ');
        iss >> freq;
        freqs.push_back(freq);
        //cout << "_ " << freq << endl;
        }
        else{
        iss >> chara >> freq;
        chars.push_back(chara);
        freqs.push_back(freq);
        //cout << chara << " " << freq << endl;
        }
    }
    //cout << endl;

    // Input redirection for binary codes and positions
    vector<int> positions;
    string cmd;

    // Vectors for threads later
    vector <string> binaryCodes;
    vector < vector <int> > all_positions;
    int messageSize = 0;

    for (int i = 0; i < size; i++){
        getline(cin,cmd);
        //getline(input, cmd);

        // binaries
        string code;
        int pos;
        istringstream cmdIss(cmd);
        cmdIss >> code;
        //cout << code << " ";

        // positions
        while (cmdIss >> pos){
            positions.push_back(pos);
            messageSize++;
            //cout << pos << " ";
        }
        //cout << endl;

        // vectors
        binaryCodes.push_back(code);
        all_positions.push_back(positions); 
        positions.clear();
    }
    
    // Build Huffman Tree
    Node *root = buildHuffman(chars, freqs, size);
    
    // output container
    vector<char> output(messageSize);

    // thread variables and semaphore variables
    Decompress arguement;
    vector<pthread_t> pthreads;
    int Turn = 0;
    static pthread_mutex_t binarySemaphore;
    static pthread_mutex_t binarySemaphore2;
    static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER;
    pthread_mutex_init(&binarySemaphore, NULL); // Initialize bsem to 1
    pthread_mutex_init(&binarySemaphore2, NULL); // Initialize bsem2 to 1

    // Create "size" many threads (Thread Creation)
    for (int i = 0; i < size; i++){
        pthread_t curThread;
        pthread_mutex_lock(&binarySemaphore);
        
        // Every loop we change the arguments stuct info
        arguement.top = root;
        arguement.code = binaryCodes[i];
        arguement.positions = all_positions[i];
        arguement.beginning = output.data();
        arguement.threadNumber = i;
        arguement.turn = &Turn;
        arguement.bsem = &binarySemaphore;
        arguement.bsem2 = &binarySemaphore2;
        arguement.waitTurn = &waitTurn;
        
        pthread_create(&curThread, nullptr, decompressor, &arguement);
        pthreads.push_back(curThread);
    }
    
    // Join the threads
    for(int i = 0; i < size; i++){
        pthread_join(pthreads[i], nullptr);
    }
    
    cout << "Original message: ";
    for (int i = 0; i < output.size(); i++){
        cout << output[i];
    }
}
