#include "router.h"


void router::ManagerConnection(char * ip,int udpPort){
    //int buffSize = 500;
    //char buff[buffSize];
    
    clientSock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(clientSock < 0){
        cerr << "ERROR CREATING CLIENT SOCKET" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Router socket created" << endl;
    
    struct sockaddr_in ServAddr;
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = inet_addr(ip);
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
    cout << "Ready for connectivity table" << endl;
    cout << "My UDP Port: " << udpPort << endl;
}

int main(int argc, char *argv[]) {
    
	cout<<"*********INSIDE ROUTER*********"<<endl;
    router router;
    
    router.udpPort = atoi(argv[1]);
    router.ipAddress = argv[2];
    router.nodeAddress = atoi(argv[3]);
    cout << "nodeAddress: " << router.nodeAddress << endl;
    router.ManagerConnection(router.ipAddress,router.udpPort);
	return 0;
}
