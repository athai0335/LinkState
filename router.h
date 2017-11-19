#ifndef ROUTER_H_INCLUDED
#define ROUTER_H_INCLUDED

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
#include <sys/time.h>

using namespace std;



class Router {
	public:
		void routerProcess();
		void sendToManager( char* message);
		char* receiveFromManager();
		void writeToRouterFile(string filename, string message);
		void sendToRouter(char* message,sockaddr_in serverUDP_addr); 
		//------------
		ofstream routerFile;

	private:
		//------------
		struct routes{
			int nodeAddress; //nodeAddress(x) and nextHop(y) are node number between 0 and N-1 
			int nextHop;
			int cost; //cost(c) of a link between x and y
		};

		vector<routes> routerInfo; //to store the X,Y,C of each nodes
		vector<int> neighbors; //to store neighbors
        
        struct routerAndPort{
            int node;
            int udpPort;
        };
        
        vector<routerAndPort> routerPortTable;
		//--------------------
};











#endif
