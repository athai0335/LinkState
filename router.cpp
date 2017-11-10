#include "router.h"

int main(int argc, char *argv[]) {

	cout<<"*********INSIDE ROUTER*********"<<endl;
    
    int clientSock;
    int buffSize = 500;
    char buff[buffSize];
    bool quit = false;
    
    clientSock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(clientSock < 0){
        cerr << "ERROR CREATING CLIENT SOCKET" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Router socket created" << endl;
    
    struct sockaddr_in ServAddr;
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = inet_addr(argv[2]);
    ServAddr.sin_port = htons(5001);
    
    cout << "Connecting to manager..." << endl;
    if(connect(clientSock,(struct sockaddr*)&ServAddr, sizeof(ServAddr)) < 0){
     cout << "ERROR IN CONNECT" << endl;
     close(clientSock);
     exit(EXIT_FAILURE);
    }
    //memset(&ServAddr,0,sizeof(ServAddr));
   // recv(clientSock,buff,buffSize,0);
    cout << "Connected to manager!" << endl;
    cout << "Ready for node address assignment and connectivity table" << endl;

	return 0;
}
