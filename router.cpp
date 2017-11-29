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
int receivedRouter[100];
int receivedLinkCount[100];
int messageReturn = 0;
string messageReceived;

vector<string> messageTosend;
int routerUDPports[MAXRoutes];
int neighborAddress;
int totalRouters;
string linkReceived;
vector<int> receivedLinkedRequestCount;

int portReceived;


vector<string> messageReceivedVec;

//*************************************************
void Router::writeToRouterFile(string filename, string message){
	routerFile.open(filename, std::ofstream::out | std::ofstream::app);
	routerFile << dateTime <<message<<endl;
	routerFile.close();
}
//*******************************************
/*bool Router::isLinkEtablishmentComplete(){
	bool status = false;
	//---------------------------
	for(int unsigned i=0; i< newRouterInfo.size(); i++){
		//cout<<"****************receivedLinkedRequestCount["<<i<<"]: "<<receivedLinkedRequestCount[i]<<endl;
		receivedLinkedRequestCount.push_back(1);

	}

	
	//-------------------------
	//cout<<"********************receivedLinkedRequestCount.size(): "<<receivedLinkedRequestCount.size()<<endl;
	//cout<<"*****************************************newRouterInfo.size(): "<<newRouterInfo.size()<<endl;
	if(receivedLinkedRequestCount.size() == newRouterInfo.size()){

		status = true;
	}
	//cout<<"_____________________"<<endl;
	return status;
}*/

//*******************************************
bool Router::isLinkEtablishmentComplete(){
	bool status = false;

	int count = 1;

	for(int i = 0; i < totalRouters; i++)
	{	
		//cout<<"*************receivedRouter["<<i<<"]: "<<receivedRouter[i]<<endl;
		
		if(receivedRouter[i] == 1){
				    count++;
		}
	
	}
	cout<<"**********************************************nodeAddress: "<<nodeAddress<<endl;
	//cout<<"************************************totalRouters: "<<totalRouters<<endl;
	cout<<"*********************count: "<<count<<endl;

	 if(count == totalRouters)
	    {
		//cout<<"*********************/////count: "<<count<<endl;
		status = true;


	    }

	/*//---------------------------
	//cout<<"************receivedRouter: "<<sizeof(receivedRouter)/sizeof(receivedRouter[0])<<endl;
	for(int unsigned i=0; i< sizeof(receivedRouter)/sizeof(receivedRouter[0]); i++){
		//cout<<"*************receivedRouter["<<i<<"]: "<<receivedRouter[i]<<endl;
		if(receivedRouter[i] == 1){
			receivedLinkedRequestCount.push_back(1);
		}
	}

	//cout<<"***********************Size: "<<receivedLinkedRequestCount.size()<<endl;
	//cout<<"***********************totalRouters: "<<totalRouters<<endl;
	//-------------------------
	if(receivedLinkedRequestCount.size() == (unsigned)totalRouters){
		//cout<<"*************complete*******\n";
		status = true;
	}*/
	cout<<"_____________________"<<endl;

	return status;
}
//*******************************************

//************************************************
void Router::performLimitedBroadcast(char* message){
//char buf[255];
	//cout<<"***************************size: "<<routerInfo.size()<<endl;
	for(int unsigned i = 0; i < routerInfo.size(); i++)
	{

			/*bzero(buf, sizeof(buf));
			sprintf(buf, "[Router%d]: %s", nodeAddress, message);
			cout<<buf<<endl;*/

		if(routerInfo.at(i).neighborPort != portReceived)
		{	
			/*cout<<"****************************************Node: "<<routerInfo.at(i).nodeAddress<<endl;
			cout<<"***************routerInfo.at(i).neighborPort: "<<routerInfo.at(i).neighborPort<<endl;
			cout<<"********************routerInfo.at(i).nextHop: "<<routerInfo.at(i).nextHop<<endl;
			cout<<"************************************neighbor: "<<neighborAddress<<endl;
			cout<<"*********************message: "<<message<<endl;*/


			neighborAddress = routerInfo.at(i).nextHop;

			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Router%d]: Forwarding Link State Packet to Neighbor Router[%d] - %s", nodeAddress, neighborAddress, message);
			writeToRouterFile(RouterFileName, messageHolder);
	
			sendToNeighbor(message);
			

		}
	}
	//cout<<"_________________________________"<<endl;


}

//********************************************
void Router::filterOutDuplicatePacket()
{
	int nodeAddress2=0;
	int nextHop=0;
	char buffer[255];

	//---------------------------------
	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: Routing Table after filtering out duplicates", nodeAddress);
	writeToRouterFile(RouterFileName, messageHolder);

	//------------------------------
	for(int unsigned i = 0; i < newRouterInfo.size(); i++)
	{

		nodeAddress2 = newRouterInfo[i].nodeAddress;
		nextHop = newRouterInfo[i].nextHop;
		for(int unsigned j = i + 1; j < newRouterInfo.size(); j++)
		{
		    if((newRouterInfo[j].nodeAddress == nextHop) && (newRouterInfo[j].nextHop == nodeAddress2))
		    {
			newRouterInfo.erase(newRouterInfo.begin() + j);
		    }
		    if((newRouterInfo[j].nodeAddress == nodeAddress2) && (newRouterInfo[j].nextHop == nextHop))
		    {
			newRouterInfo.erase(newRouterInfo.begin() + j);
		    }

		}
	}

	//-------------------------------------
 	for(int unsigned i = 0; i < newRouterInfo.size(); i++)
	{
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "[Router%d]: 	|Node: %d |Dest: %d |cost: %d|",nodeAddress, newRouterInfo[i].nodeAddress,newRouterInfo[i].nextHop,newRouterInfo[i].cost);
		//cout<<"*******************buffer: "<<buffer<<endl;
		writeToRouterFile(RouterFileName, buffer);
	}
	//cout<<"_______________________________________________________"<<endl;
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
void Router::sendToNeighbor(char* message){
	//src: http://beej.us/guide/bgnet/output/html/multipage/sendman.html	
	

	struct sockaddr_in destRouterAddr;
	int port =0;
	//---------type of socket created-----------------
	memset(&destRouterAddr, 0, sizeof(destRouterAddr));
	destRouterAddr.sin_family = AF_INET;
	destRouterAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//cout<<"***********************size: "<<routerInfo.size()<<endl;
	for(int unsigned i=0; i<routerInfo.size(); i++){
		
		if(routerInfo.at(i).nodeAddress == nodeAddress){
			//cout<<"**********nodeAddress: "<<routerInfo.at(i).nodeAddress<<endl;
			//cout<<"**********neighborPort: "<<routerInfo.at(i).neighborPort<<endl;

			port = routerInfo.at(i).neighborPort;
			destRouterAddr.sin_port = htons(port);
			//----------------------------------------
			int returnVal = sendto(routerUDPsock, message, strlen(message)+1, 0, (struct sockaddr*)&destRouterAddr, sizeof(destRouterAddr));
			if (returnVal < 0){
				//cout<<"Error: Router enable to send message to neighbor"<<endl;
				perror("Router enable to send message to neighbor");
				exit(1);
			}
	
		}
		
	}

	
	//----------------------------------
	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Router%d]: 	Message sent successfully!", nodeAddress);
	writeToRouterFile(RouterFileName, messageHolder);



}
//*********************************************************************
void Router::receiveFromNeighbor(){
	
	//----------------------	
	char buffer[255];
	 //setup address for udp sender
	struct sockaddr_in udpSender;
	socklen_t addrlen = sizeof(udpSender);
	//recieve Link request
	bzero(buffer,255); //clear the buffer
	if(recvfrom(routerUDPsock,buffer,sizeof(buffer),0,(struct sockaddr*)&udpSender,&addrlen) < 0){
		perror("ERROR RECEIVING UDP STATUS: ");
	}
	//cout<<"**********buffer: "<<buffer<<endl;	
	//---------------
	portReceived = htons(udpSender.sin_port);
	//cout<<"**************************portReceived: "<<portReceived<<endl;
	
	//------------
	linkReceived = buffer;

	//--------Making sure the router does not forward a packect to itself----------

	string temp = buffer;
	int startPos = temp.find("[")+7;
	string routerNum = temp.substr(startPos, temp.find("]")-startPos);
	//cout<<"*****************routerNum: "<<atoi(routerNum.c_str())<<endl;
	//cout<<"***************nodeAddress: "<<nodeAddress<<endl;
	
	if(atoi(routerNum.c_str()) != nodeAddress){

		//cout<<"**********buffer: "<<buffer<<endl;	
		

		bzero(messageHolder, sizeof(messageHolder));
		sprintf(messageHolder, "[Router%d]: 	Received from Neighbor %s", nodeAddress,buffer);
		writeToRouterFile(RouterFileName, messageHolder);

		//--------------------
		/*routes tempNodeInfo;
		for(int unsigned i = 0; i < routerInfo.size(); i++)
		{	
			tempNodeInfo.nodeAddress = routerInfo.at(i).nodeAddress;
			newRouterInfo.push_back(tempNodeInfo);

			tempNodeInfo.nextHop = routerInfo.at(i).nextHop;
			newRouterInfo.push_back(tempNodeInfo);

			tempNodeInfo.cost = routerInfo.at(i).cost;
			newRouterInfo.push_back(tempNodeInfo);

			tempNodeInfo.neighborPort = routerInfo.at(i).neighborPort;
			newRouterInfo.push_back(tempNodeInfo);
		}*/

		//--------------------
		cout<<"//////////////////////////////////////////////////////////////////////////Router: "<<nodeAddress<<endl;
				//cout<<"**********buffer: "<<buffer<<endl;	

		//char temBuf [255];
		string holder = buffer;			
		//cout<<"***********************holder: "<<holder<<endl;
		int startPos = holder.find("-")+2;
		string newMessage = holder.substr(startPos);
		//cout<<"***********************newMessage: "<<newMessage<<endl;

		//----------------
		routes nodeInfo2;
		string token;
		string tok;
		istringstream iss(newMessage);
		while(getline(iss, token, '|')){ 
			//cout<<"**********token: "<<token<<endl;
			istringstream iss2(token);
			while(getline(iss2, tok, ' ')){ 
				//cout<<"**********tok x: "<< tok<<endl;
				nodeInfo2.nodeAddress = atoi(tok.c_str());

				//--------------------
				getline(iss2, tok, ' ');
				//cout<<"*********tok y: "<<tok<<endl;
				nodeInfo2.nextHop = atoi(tok.c_str());

				//-------------------
				getline(iss2, tok, ' ');
				//cout<<"*********tok c: "<<tok<<endl;
				nodeInfo2.cost = atoi(tok.c_str());

				//-------------------
				getline(iss2, tok, ' ');
				//cout<<"*********tok port: "<<tok<<endl;
				nodeInfo2.neighborPort = atoi(tok.c_str());

				//--------------
				newRouterInfo.push_back(nodeInfo2);
				//--------------
				//mapOfRouterInfos.push_back(newRouterInfo);
				
			}
		}
	}
	//cout<<"__________________________________________________"<<endl;
	//----------------------

	//cout<<"******************************************************************newRouterInfo.size(): "<<routerInfo.size()<<endl;
	for(int unsigned i = 0; i < newRouterInfo.size(); i++)
	{
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "[Router%d]: 	|Node: %d |Dest: %d |cost: %d|",nodeAddress, newRouterInfo[i].nodeAddress,newRouterInfo[i].nextHop,newRouterInfo[i].cost);
		cout<<"*******************buffer: "<<buffer<<endl;

		//writeToRouterFile(RouterFileName, buffer);
	}
	//cout<<"************************************************size: "<<mapOfRouterInfos.size()<<endl;

	cout<<"----------------------------------------------------------------------------------"<<endl;
	
	


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

	//cout<<dateTime<<"[Router"<<nodeAddress<<"]: Process started"<<endl;
	//cout<<"	"<<dateTime <<"check router"<<nodeAddress<<".out for process status\n"<<endl;
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

	 //----bind--------------------
	if(bind(routerUDPsock,(struct sockaddr *)&serverUDP_addr,sizeof(serverUDP_addr)) < 0){
		perror("bind in udp failed");
	}

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
			//if((messageReturn == 0)){
		
				//----------------receive node Address and connectivity table from Manager-----------------------------------------
				
				receiveFromManager(); 
		
				//---------------
				messageTosend.push_back(messageReceived);
				//cout<<"*****************messageReceived NodeAddress: "<<messageReceived<<endl;

				//-------------Extract informations from the message received--------------
				routes nodeInfo;	
				
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

							//-------------------
							getline(iss2, tok, ' ');
							//cout<<"*********tok port: "<<tok<<endl;
							nodeInfo.neighborPort = atoi(tok.c_str());

							//--------------
							routerInfo.push_back(nodeInfo);
							
						}
				}
				
				if(messageReceived == "ALL_COMPLETE!"){

					filterOutDuplicatePacket();

					
				}
				else{
					//-----------------------------------send ready message to manager-----------------------------------------
					bzero(buffer,255); //clear the buffer
	      				sprintf(buffer, "%s", "READY!");
					sendToManager(buffer);
				}



				
			
				
				//-------Wait for the manager to indicate that all links from all routers have been established and the network is up------
				
				receiveFromManager(); 

				//-----------
				if(messageReceived == "NETWORK_IS_UP!"){
					//-------------
					string msg ="";
					for(int unsigned i=0; i<routerInfo.size(); i++){
						neighborAddress = routerInfo.at(i).nextHop;

						msg = messageTosend.at(0).c_str();


						bzero(messageHolder, sizeof(messageHolder));
						sprintf(messageHolder, "[Router%d]: Sending Link State Packet to Neighbor Router[%d] - %s", nodeAddress, neighborAddress, msg.c_str());
						writeToRouterFile(RouterFileName, messageHolder);

					}

					//-------------
					bzero(buffer,255); //clear the buffer
	      				sprintf(buffer, "[Router%d] - %s", routerInfo.at(0).nodeAddress,messageTosend.at(0).c_str());

					sendToNeighbor(buffer);

					
						
					
				}
				
			

		}

		//--If something happened on the Router UDP socket , 
		//--then its an incoming connection from another router----
		if (FD_ISSET(routerUDPsock, &readfds))  
		{ 
			
				vector<int> vecOfNodeAddress;
				//--------				
				receiveFromNeighbor();
				//cout<<"**********************nodeAddress: "<<nodeAddress<<endl;
				//cout<<"*****************linkReceived: "<<linkReceived<<endl;
				//-------Extract the router number----------
				//cout<<"************linkReceived: "<<linkReceived<<endl;
				int startPos = linkReceived.find("[")+7;
				string routerNumRecieved = linkReceived.substr(startPos, linkReceived.find("]")-startPos);
				//cout<<"***********routerNumRecieved: "<<atoi(routerNumRecieved.c_str())<<endl;

				//------keep track of routers that received a link request-----------
				receivedRouter[atoi(routerNumRecieved.c_str())] = 1;
			
				//-----------Perform limited broadcast on all LSPs received from other routers-----

				bzero(buffer,255); //clear the buffer 
	      			sprintf(buffer, "%s", linkReceived.c_str());
				performLimitedBroadcast(buffer);
				


				if(isLinkEtablishmentComplete()){
					char buffer[255];
					bzero(buffer, sizeof(buffer));
					sprintf(buffer, "COMPLETE!");
					sendToManager(buffer);
				}
				
			
			

		
		}


	}


	
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
	totalRouters = atoi(argv[5]);
	


	Router router;
	router.routerProcess();
	
	

	return 0;
}

