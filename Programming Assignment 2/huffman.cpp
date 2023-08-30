// Programming Assignment 2 (huffmanTree.cpp)

#include "huffmanTree.h"

// HUFFMAN STUFF

//Used to print out the leafs from left to right and insert into symbolCodes map
void printCodes(struct Node* root, string str){
  if (!root)
    return;
  
  if (!root->left && !root->right){
    cout << "Symbol: " << root->character << ", Frequency: " << root->frequency << ", Code: " << str << endl;
  }
  
  printCodes(root->left, str + "0");
  printCodes(root->right, str + "1");
}

// Used to find a certain binary code and return the character linked to it
void traversal(Node *root, string str, string code, char &result){
  if (!root)
    return;
  if (str == code){
    result = root->character;
    return;
  }
  traversal(root->left, str + "0", code, result);
  traversal(root->right, str + "1", code, result);
}

// Used to build the huffman tree
Node* buildHuffman(vector<char> c, vector<int> f, int size){
  
  //intialize our pointers in the tree
  Node *left, *right, *top;

  // Create a min heap pq & inserts all characters and their frequencies
  priority_queue<Node*, vector<Node*>, compare> pq;
  int count = 0;

  for (int i = 0; i < size; i++){
    pq.push(new Node(c[i], f[i], count++));
  }
  
  // Iterate while size of heap doesn't become 1
  while (pq.size() != 1) {

    // Extract the two minimum freq items from min heap
    left = pq.top();
    pq.pop();

    right = pq.top();
    pq.pop();

    // Create a new internal node with frequency equal to the
    // sum of the two nodes frequencies. Make the two extracted
    // node as left and right children of this new node. Add
    // this node to the min heap ' ' is a special value for
    // internal nodes, not used. (used from geeks for geeks)
    top = new Node(' ', left->frequency + right->frequency, count++);

    top->left = left;
    top->right = right;

    pq.push(top);
  }

  // Print Huffman codes using the Huffman tree built above
  printCodes(pq.top(), "");
  return pq.top();
}
