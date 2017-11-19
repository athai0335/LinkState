#include "router.h"
#include "manager.h"


int udpPort;
int tcpPort;
int routerTCPsock;
int routerUDPsock;
int nodeAddress;
string managerIP;
string dateTime;
char RouterFileName[255];
char messageHolder[255];
int messageReturn = 0;
string messageReceived;
//*************************************************
//------------------
void Router::writeToRouterFile(string filename, string message){
	
	routerFile.open(filename, std::ofstream::out | std::ofstream::app);
	routerFile << dateTime <<message<<endl;
	routerFile.close();
}


//******************Use to send message to manager*******************
void Router::sendToManager( char* message){
	//cout<<""<<dateTime<<"[Router]: sending to manager - " <<message <<endl;
	

	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: Sending to manager - %s", nodeAddress,message);
	writeToRouterFile(RouterFileName, messageHolder);
	//---------------------
	int returnVal = send(routerTCPsock,message,strlen(message),0);
	if (returnVal < 0){
		cout<<"Error: Router enable to send message to Manager"<<endl;
		exit(1);
	}
	//------------------
	//cout<<""<<dateTime<<"[Router]: message sent successfully!" <<endl;
	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: 	Message sent successfully!", nodeAddress);
	writeToRouterFile(RouterFileName, messageHolder);
}

//******************Use to send message to other routers*******************
void Router::sendToRouter(char* message){
	//----------------
	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: Sending to neighbors - %s", nodeAddress,message);
	writeToRouterFile(RouterFileName, messageHolder);
	
	//----------------------
	/*int returnVal = send(routerUDPsock,message,strlen(message),0);
	if (returnVal < 0){
		cout<<"Error: Router enable to send message to neighbor"<<endl;
		exit(1);
	}*/
	//--------------------


}



//***************use to receive message from the manager****************
char* Router::receiveFromManager(){
	//cout<<""<<dateTime<<"[Router]: receiving from manager..."<<endl;
	
	char buffer[255];
	bzero(buffer,255); //clear the buffer
	 
	//-----------------
	recv(routerTCPsock, buffer, sizeof(buffer), 0);
	//---------------------

	//----------Formatting the output file-----------------------------------
	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: Receiving from manager...", nodeAddress);
	writeToRouterFile(RouterFileName, messageHolder);	

	//---------------------------
	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: 	Received from manager - %s", nodeAddress, buffer);
	writeToRouterFile(RouterFileName, messageHolder);

	
	//--------------------------
	char* str = (char *)malloc(sizeof(char) * 256);
	memset(str, '\0', sizeof(buffer) + 1);
	strncpy(str, buffer, sizeof(buffer));

	//---------------------
	messageReceived = buffer;

	return str;
} 

//*****************process for each routers***************************
void Router::routerProcess(){

	cout<<dateTime<<"[Router"<<nodeAddress<<"]: Process started"<<endl;
	cout<<"	"<<dateTime <<"check router"<<nodeAddress<<".out for process status\n"<<endl;
	//------------------------
	
	bzero(RouterFileName, sizeof(RouterFileName));
	sprintf(RouterFileName, "router%d.out",nodeAddress);

	//--------clear the router file before writing the new output
	routerFile.open(RouterFileName, std::ofstream::out | std::ofstream::trunc);
	routerFile.close();
	//---------------

	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: Process started",nodeAddress);
	
	writeToRouterFile(RouterFileName, messageHolder);	



	//------varaiables---------
	int activity , max_sd; //used for select()
	//int sd;
	char buffer[255];
	//----------create set of socket descriptors-----------------
	fd_set readfds;
	

	//-----------create a TCP socket for the router to communicate with the Manager-------
  	//cout<<""<<dateTime<<"[Router]: creating TCP socket..."<<endl;

	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: creating TCP socket...", nodeAddress);
	
	writeToRouterFile(RouterFileName, messageHolder);
	//-------------------------------------
	
    	struct sockaddr_in serverTCP_addr;

	routerTCPsock = socket(AF_INET, SOCK_STREAM,0);

	if (routerTCPsock < 0) {
		fprintf(stderr, "Error: Router failed to create TCP socket\n");
		exit(1);
	}
	
	//-------------------
	//cout<<""<<dateTime<<"[Router]: TCP socket created."<<endl;

	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: TCP socket created!", nodeAddress);
	
	writeToRouterFile(RouterFileName, messageHolder);

	//----------
	//cout<<""<<dateTime<<"[Router]: TCP Port: "<<tcpPort<<endl;
	
	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: TCP Port - %d", nodeAddress, tcpPort);
	
	writeToRouterFile(RouterFileName, messageHolder);

	//----------set router TCP socket to allow multiple connections (Good habit)------------ 
	int opt = 1;
	if(setsockopt(routerTCPsock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) <0){
		fprintf(stderr, "Router TCP: setsockopt failed\n");
		exit(1);
	}


	//---------type of socket created-----------------
	memset(&serverTCP_addr, 0, sizeof(serverTCP_addr));
	serverTCP_addr.sin_family = AF_INET;
	serverTCP_addr.sin_addr.s_addr = inet_addr(managerIP.c_str());
	serverTCP_addr.sin_port = htons(tcpPort);



	//--------create a UDP socket for the router to communicate among each other-------------
	//cout<<""<<dateTime<<"[Router]: creating UDP socket..."<<endl;
		
	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: creating UDP socket...", nodeAddress);
	writeToRouterFile(RouterFileName, messageHolder);


	//-----------------------
	routerUDPsock = socket(AF_INET, SOCK_DGRAM,0);

	if (routerUDPsock < 0) {
		fprintf(stderr, "Error: Router failed to create UDP socket\n");
		exit(1);
	}
	//------------------
	//cout<<""<<dateTime<<"[Router]: UDP socket created."<<endl;

	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: UDP socket created!", nodeAddress);
	writeToRouterFile(RouterFileName, messageHolder);

	//----------------------
	//cout<<""<<dateTime<<"[Router]: UDP Port: "<<udpPort<<endl;
	
	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: UDP Port - %d", nodeAddress,udpPort);
	writeToRouterFile(RouterFileName, messageHolder);	

	//----------set router UDP socket to allow multiple connections (Good habit)------------ 
	int opt2 = 1;
	if(setsockopt(routerUDPsock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt2, sizeof(opt2)) <0){
		fprintf(stderr, "Router UDP: setsockopt failed\n");
		exit(1);
	}
	//---------type of socket created-----------------
	sockaddr_in serverUDP_addr;

	memset(&serverUDP_addr, 0, sizeof(serverUDP_addr));
	serverUDP_addr.sin_family = AF_INET;
	serverUDP_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverUDP_addr.sin_port = htons(udpPort);


	//-----Connect the router TCPsocket to the manager server socket-----------
	//cout<<""<<dateTime<<"[Router]: Connecting to manager <"<<inet_ntoa(serverTCP_addr.sin_addr)<<">..."<< endl;

	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: Connecting to manager <%s>...", nodeAddress,inet_ntoa(serverTCP_addr.sin_addr));
	writeToRouterFile(RouterFileName, messageHolder);


	if (connect(routerTCPsock, (struct sockaddr *) &serverTCP_addr, sizeof(serverTCP_addr)) < 0){
		fprintf(stderr, "Error: Router Unable to connect\n");
		pthread_exit(NULL);
	}

	//-----------------
    	//cout<<""<<dateTime<<"[Router]: connected succssuflly to manager!"<<endl;

	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: connected succssuflly to manager!", nodeAddress);
	writeToRouterFile(RouterFileName, messageHolder);

	//--------------sending the UDP port to manager---------------
	char udpPortBuffer[255];
	bzero(udpPortBuffer,255); //clear the buffer

        sprintf(udpPortBuffer, "Router[%d] UDP Port - %d", nodeAddress, udpPort);
	sendToManager(udpPortBuffer);



	while(1)
	{
		//--------clear the socket set-------------
		FD_ZERO(&readfds);
		
		//------add router TCP socket to set--------------
		FD_SET(routerTCPsock, &readfds);

		//------add router UDP socket to set--------------
		FD_SET(routerUDPsock, &readfds);
		
		max_sd = routerTCPsock;

		//--highest file descriptor numner, 
		//--need it for the select function--
		if(routerUDPsock > max_sd){
			max_sd = routerUDPsock;
		}

		//--wait for an activity on one of the sockets , timeout is NULL , 
		//--so wait indefinitely 

		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  

		if ((activity < 0) && (errno!=EINTR))  
		{  
		   cout<<"Router select error"<<endl;
		}  
		    
		//--If something happened on the Router TCP socket , 
		//--then its an incoming connection from manager--------
		if (FD_ISSET(routerTCPsock, &readfds))  
		{ 

			switch(messageReturn){
				case 0 :
				{
					//----receive node Address and connectivity table from Manager
					 receiveFromManager(); 
					//-----------------------------
					
					//-------------Extract informations from the message received--------------
					routes nodeInfo;	
					//cout<<"*********messageReceived: "<<messageReceived<<endl;
					
					string token;
					string tok;
					istringstream iss(messageReceived);
					while(getline(iss, token, '|')){ 
						//cout<<"**********token: "<< token<<endl;
							istringstream iss2(token);
							while(getline(iss2, tok, ' ')){ 
								//cout<<"**********tok x: "<< tok<<endl;
								nodeInfo.nodeAddress = atoi(tok.c_str());

								//--------------------
								getline(iss2, tok, ' ');
								//cout<<"*********tok y: "<<tok<<endl;
								nodeInfo.nextHop = atoi(tok.c_str());

								//-------------------
								getline(iss2, tok, ' ');
								//cout<<"*********tok c: "<<tok<<endl;
								nodeInfo.cost = atoi(tok.c_str());

								//--------------
								routerInfo.push_back(nodeInfo);
								
							}
					}
					
					//Extract neighbors
					for(int i = 0; i < routerInfo.size(); i++){
                        neighbors.push_back(routerInfo.at(i).nextHop);
                    }

					//------------------------------
					/*for(int unsigned i=0; i<routerInfo.size(); i++){
						cout<<"*************nodeAddress["<<i<<"]: "<<routerInfo.at(i).nodeAddress<<endl;
						cout<<"**********nextHop["<<i<<"]: "<<routerInfo.at(i).nextHop<<endl;
						cout<<"*************cost["<<i<<"]: "<<routerInfo.at(i).cost<<endl;
					}
					cout<<"----------------------"<<endl;*/


					//--------------------------------------------

					messageReturn = 1;
				}
					break;
                    
                case 1 :
                {
                    receiveFromManager();
                    
                    routerAndPort rp;
                    //need to tokenize what is received to store node and corresponding udp port
                    string token;
					string tok;
					istringstream stream(messageReceived);
                    //cout << "MESSAGE RECEIVED: " << messageReceived << endl;
                    //cout << "--------------" << endl;
					while(getline(stream, token, '|')){ 
                        //cout << "TOKEN^^^^^^^^^^^^^: " << token << endl;
                        istringstream stream2(token);
                        while(getline(stream2,tok,' ')){
                            //cout << "TOK#########: " << tok << endl;
                            rp.node = atoi(tok.c_str());
                            
                            getline(stream2,tok,' ');
                            //cout << "TOK@@@@@@@@@: " << tok << endl;
                            rp.udpPort = atoi(tok.c_str());
                            
                            routerPortTable.push_back(rp);
                        }
                    }
                    //-----send ready message to manager-------      
					bzero(buffer,255); //clear the buffer
                    sprintf(buffer, "%s", "READY!");
					sendToManager(buffer);
                    messageReturn = 2;
                }
                    break;
				
				case 2 :
                {
					//----receive ACK from manager that it is save to reach neighbors-----------------
					receiveFromManager(); 
                    
                    //----bind--------------------
                    if(bind(routerUDPsock,(struct sockaddr *)&serverUDP_addr,sizeof(serverUDP_addr)) < 0){
                        perror("bind in udp failed");
                    }

					//-----send a link request to each neighbor and wait for an ACK------
					bzero(buffer,255); //clear the buffer
		      			sprintf(buffer, "%s", "Sending link request");
					sendToRouter(buffer);
					
			
					//----------------------------
                    messageReturn = 3;
                }
                    break;
			}

		}

		//--If something happened on the Router UDP socket , 
		//--then its an incoming connection from another router----
		if (FD_ISSET(routerUDPsock, &readfds))  
		{ 
			
			switch(messageReturn){
				case 1 :

					cout<<"********working******\n";

			}
		}


	}








	//------------
	//receiveFromManager();
	
}








//***************************************************************************************
int main(int argc, char *argv[]) {
	
	//cout<<endl;
	//cout<<"*********Hello World! from router*********"<<endl;
	//----------	

	Manager manager;
	dateTime = manager.currentDateTime();
	
	//---------------
	udpPort = atoi(argv[1]);
	tcpPort = atoi(argv[2]);
	nodeAddress = atoi(argv[3]);
	managerIP = argv[4];

	Router router;
	router.routerProcess();
	
	

	return 0;
}
