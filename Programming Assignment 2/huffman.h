// Programming Assignment 2 (huffmanTree.h)

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <map>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

using namespace std;

// Huffman Tree node
struct Node {
    char character;
    int frequency;
    int position;
    Node *left, *right;

    Node(char c, int f, int pos) {
        character = c;
        frequency = f;
        position = pos;
        left = right = nullptr;
    }
};

// Used to compare nodes in the Huffman Tree
struct compare {
    bool operator()(Node* l, Node* r) {
        if (l->frequency == r->frequency) {
            if (l->character == r->character)
                return l->position < r->position;
            return l->character > r->character;
        }
        return (l->frequency > r->frequency);
    }
};

// Used to print out the leafs from left to right and insert into symbolCodes map
void printCodes(struct Node* root, string str);

// Used to find a certain binary code and return the character linked to it
void traversal(Node *root, string str, string code, char &result);

// Used to build the Huffman tree
Node* buildHuffman(vector<char> c, vector<int> f, int size);
