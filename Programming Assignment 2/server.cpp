// Programming Assignment 2 (server.cpp)

#include <fstream>

#include <iostream>
#include <sstream>
#include "huffmanTree.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
using namespace std;

void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[]){
    //Commment out when working
    //ifstream input("input2.txt");

    // Input redirection and put characters and frequencies into vectors
    string inStr;
    vector <char> chars;
    vector <int> freqs;
    
    while (getline(cin, inStr)){
    //while (getline(input, inStr)){
        char chara;
        int freq;
        istringstream iss(inStr);
        if (inStr.at(0) == ' '){
        chars.push_back(' ');
        iss >> freq;
        freqs.push_back(freq);
        }
        else{
        iss >> chara >> freq;
        chars.push_back(chara);
        freqs.push_back(freq);
        }
    }
    int mes_size = 0;
    for (int i = 0; i < freqs.size(); i++){
        mes_size += freqs[i];
    }
    int size = chars.size();

    // Build Huffman Tree
    Node *root = buildHuffman(chars, freqs, size);

/*-----------------------------------------------------------------------------------------------------------------------------------*/ 
    // Code from blackboard (modified)

    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    // If no port number
    if (argc < 2) {
        cerr << "ERROR, no port provided";
        _exit(-1);
    }

    // If socket creation fails
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "ERROR opening socket";
        _exit(-1);
    }

    // Sets server address
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]); // Sets port number to int
    serv_addr.sin_family = AF_INET; // AF_INET = Server's address is IPv4.
    serv_addr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY = Server will listen to all network interfaces.
    serv_addr.sin_port = htons(portno);

     // If bind doesn't asscociate the socket and the port number and fails
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        cerr << "ERROR on binding";
        _exit(-1);
    }
              
    // Enabling the socket to recieve incoming connections from a client connections with a maximum of 5 clients in a waiting in a queue
    listen(sockfd, 10);
    clilen = sizeof(cli_addr); // Size of client address
    
    signal(SIGCHLD, fireman);
    while (true){
        // Blocks until a client connection is made, when a connection is made accept returns a new socket (newsockfd) that is connected to the client
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);

        if (fork() == 0)
        {
            if (newsockfd < 0)
            { 
                cerr << "ERROR on accept";
                _exit(-1);
            }
            
            // READING
            char buffer[256];
            bzero(buffer, 256);
            n = read(newsockfd, buffer, 256);

            if (n < 0)
            {
                cerr << ("ERROR reading from socket");
                _exit(-1);
            }
            
            char result = ' ';
            traversal(root, "", buffer, result);
            char decodedChar = result;
            //cout << "Binary Code from client: " << buffer << endl;
            

            // WRITING
            bzero(buffer, 256);
            buffer[0] = decodedChar;

            n = write(newsockfd, buffer, 256);
            if (n < 0)
            {
                cerr << ("Error writing to socket");
                _exit(-1);
            }
            close(newsockfd);
            _exit(0);
        }     
    }
close(sockfd);    
return 0;
}
