#ifndef MANAGER_H_INCLUDED
#define MANAGER_H_INCLUDED

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <getopt.h>
#include<stdbool.h>
#include  <sys/types.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

#define MAX_ROUTES 128 //maximum size of routing table
#define MAX_ARGS 3 //the maximum number of argument the router is going to take. the last one hasto be NULL
#define MAXPENDING 10 //max fd's



class Manager {
	



	public:

		void readTopologyFile(string topologyFile);
		void createNetwork();
        int servSock, clientSock, max_fd, activity;
        int ServPort = 5001;
        struct sockaddr_in ServAddr;
        socklen_t size;
        pid_t child_pid;
        int child_status;
        struct hostent *temp;
        struct in_addr addr;
        fd_set readfds;

	private:
		struct routes{
			int totalRoutes;	
			vector<string> vecOfNodesInfo;
			int dest; //dest(x) and nextHop(y) are node number between 0 and N-1 
			int nextHop;
			int cost; //cost(c) of a link between x and y
		};
		//------------
		struct packetInfo{
			int src;
			int dst;
		};
		//------------
		vector<packetInfo> packet; //to store the source and destination pair of the packet
		//-----------
		vector<routes> route; //to store the X,Y,C of each nodes

};











#endif
