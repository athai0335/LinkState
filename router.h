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
};











#endif
