// Programming Assignment 2 (client.cpp)

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <pthread.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
using namespace std;

// Decompress Stuff (some of this stuff from client.c)

// Decompressed Node
struct Decompress {
  string code;
  vector <int> positions;
  char *beginning;
  char *hostname;
  int portno;

  Decompress (string Code, vector <int> Positions, char *Beginning, char *Hostname, int Portno){
    code = Code;
    positions = Positions;
    beginning = Beginning;
    hostname = Hostname;
    portno = Portno;
  }
};

// Decompressor Stuff (some of this stuff from client.c)
void *decompressor(void *arg){
  struct Decompress *arguments = (Decompress*) arg;

  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  // Set the server from the hostname
  server = gethostbyname(arguments->hostname);
  
  // Set port number
  portno = arguments->portno;

  // If socket creation fails
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    cerr << "ERROR opening socket";
    _exit(-1);
  }

  // Sets server address
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
  
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
    cerr << "ERROR connecting";
    _exit(-1);
  }
  
  char buffer[256];
  // WRITING
  bzero(buffer, 256);
  strcpy(buffer, arguments->code.c_str());
  n = write(sockfd, buffer, 256);
  if (n < 0){
    cerr << "ERROR writing to socket";
    _exit(-1);
  }

  // READING
  bzero(buffer,256);
  n = read(sockfd, buffer, 256);
  if (n < 0){
    cerr << "ERROR reading from socket";
    _exit(-1);
  }
  //cout << "Character from that code: " << buffer << endl;
  // Storing the symbols in the positions vector
  for (int i = 0; i < arguments->positions.size(); i++){
    int position = arguments->positions[i];
    *(arguments->beginning + position) = buffer[0];
  }
  close(sockfd);
  return nullptr;
}

int main(int argc, char *argv[]){

/*---------------------------------------------------------------------------------------------------------------------------*/
    //Comment out when working
    //ifstream command("compressed2.txt");

    // Code from blackboard (modified)


    // If there is no hostname and port number provided
    if (argc < 3) {
       cerr << "usage: " << argv[0] << " hostname port";
       exit(-1);
    }
    
    // Retrieves the IP address of the host from the first arguement argv[1]
    struct hostent *server;
    server = gethostbyname(argv[1]); 
    // If theres is no IP address provided
    if (server == NULL) {
        cerr << "ERROR, no such host";
        exit(-1);
    }
    
    // Command Parsing
    vector<int> positions;
    string cmdLine;
    int lines = 0;
    int messageSize = 0;

    // Vectors for threads later
    vector <string> binaryCodes;
    vector < vector <int> > all_positions;
    
    while (getline(cin,cmdLine)){
    //while (getline(command, cmdLine)) {
        string code;
        int pos;
        istringstream cmdIss(cmdLine);
        cmdIss >> code;
        lines++;

        while (cmdIss >> pos){
            positions.push_back(pos);
            messageSize++;
        }
        binaryCodes.push_back(code);
        all_positions.push_back(positions); 
        positions.clear();
    }
    
    // output container
    vector<char> output(messageSize);

    // thread variables
    static vector<Decompress *> t_args;
    vector<pthread_t> threads;

    for (int i = 0; i < lines; i++){
        // thread creation
        pthread_t thread;
        Decompress *t_arg = new Decompress(binaryCodes[i], all_positions[i], output.data(), argv[1], stoi(argv[2]));
        if (pthread_create(&thread, nullptr, decompressor, t_arg)) {
            cerr << "Cannot create thread" << endl;
            return 1;
        }
        threads.push_back(thread); // thread vector
        t_args.push_back(t_arg); // vector of Decompress* structs (pushing back each pointer)
    }

    for (int i = 0; i < threads.size(); i++) {
        pthread_join(threads[i], nullptr);
    }
    /*
    cout << "1:" << output[0] << endl;
    cout << "2:" << output[1] << endl;
    cout << "3:" << output[2] << endl;
    cout << "4:" << output[3] << endl;
    cout << "5:" << output[4] << endl;
    cout << "6:" << output[5] << endl;
    cout << "7:" << output[6] << endl;
    cout << "8:" << output[7] << endl;
    cout << "9:" << output[8] << endl;
    */
    cout << "Original message: ";
    for (int i = 0; i < output.size(); i++){
        cout << output[i];
    }
    
return 0;
}
