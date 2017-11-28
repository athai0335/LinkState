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
#include <sys/time.h>
#include <array>
#include <fstream>
//#include <pthread.h>
#include <thread>
//#include <boost/thread.hpp>
//#include <boost/chrono.hpp>
#include <algorithm>
#include <set>

using namespace std;

#define MAX_ROUTES 128 //maximum size of routing table
#define MAX_ARGS 8 //the maximum number of argument the router is going to take. the last one hasto be NULL
#define BACKLOG 5 //how may pending connections queue will hold


const int DEFAUL_TCP_PORT = 20017;
const int FixedBufferSize = 1024;



class Manager {

	public:

		void readTopologyFile(string topologyFile);
		void createNetwork();
		void managerProcess();
		char* getManagerIPAddress();
		void sendToRouter();
		bool isRoutersConnected();
		bool isRoutersLinkComplete();
		void writeToManagerFile(string str);
		string getRouterInfo(int i);
		bool isRoutersReady();
		void sendSafeToProceed();
		void sendNetworkIsUP();
		//************* Get current date/time, format is %Y-%m-%d (%F) H:M:S (%X)**********************
		string currentDateTime(){
	
			timeval curTime;
			gettimeofday(&curTime, NULL);
			int milli = curTime.tv_usec / 1000;

			char buf [80];
			strftime(buf, sizeof(buf), "%F %X", localtime(&curTime.tv_sec));

			char currentTime[84] = "";
			sprintf(currentTime, "[%s:%d]", buf, milli);



			return currentTime;

		}


		//----------src: https://stackoverflow.com/questions/10673585/start-thread-with-member-function------

		thread createNetworkThread() {
			 return thread(&Manager::createNetwork, this);
		}

		thread managerProcessThread() {
			 return thread(&Manager::managerProcess, this);
  		}

		//------------
		ofstream managerFile;


	private:
		//------------
		struct routes{
			int totalRoutes;	
			vector<string> vecOfNodesInfo;
			int nodeAddress; //nodeAddress(x) and nextHop(y) are node number between 0 and N-1 
			int nextHop;
			int cost; //cost(c) of a link between x and y
		};

		vector<routes> route; //to store the X,Y,C of each nodes
		//--------------------

		struct packetInfo{
			int src;
			int dst;
		};

		vector<packetInfo> packet; //to store the source and destination pair of the packet


		


};











#endif

