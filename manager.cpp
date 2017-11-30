#include "manager.h"

//--------Global Variablrs-------------
string dateTime; 
int totalRoutes =0;
int routerTCPsock;
int routerTCPsocket[MAX_ROUTES]; //contains sockets for all routers
int receivedUDP[MAX_ROUTES]; //to store numbers of UDP message received from router
vector<int> receivedUDP_Message;
vector<int> receivedRouterNum;
vector<int> totalReady;
vector<int> totalLinkComplete;

int tempNodeAddress;
vector<int> vecNodeAddress;

vector<int> totalTableComplete;


//------------------
void Manager::writeToManagerFile(string str){
	
	managerFile.open("manager.out", std::ofstream::out | std::ofstream::app);
	managerFile << dateTime <<str<<endl;
	managerFile.close();
}


//*******************************************************************************
bool Manager::isRoutersConnected()
{
	bool status = false;

	if(receivedUDP_Message.size() == (unsigned) route.at(0).totalRoutes)
	{
		status = true;
	}
	else{
		status = false;
	}



	return status;
}
//*******************************************************************************
bool Manager::isRoutersReady()
{
	bool status = false;
	//cout<<"**************totalReady.size(): "<<totalReady.size()<<endl;
	if(totalReady.size() == (unsigned) route.at(0).totalRoutes){
		status = true;
	
	}
	else{
		status = false;
	}

	
	return status;
}

//*******************************************************************************
bool Manager::isRoutersLinkComplete()
{
	bool status = false;
	//cout<<"**************totalLinkComplete.size(): "<<totalLinkComplete.size()<<endl;
	if(totalLinkComplete.size() == (unsigned) route.at(0).totalRoutes){
		status = true;
	
	}
	else{
		status = false;
	}

	
	return status;
}

//******************************************************************

bool Manager::isAllTableComplete()
{

	bool status = false;
	//cout<<"**************totalTableComplete.size(): "<<totalTableComplete.size()<<endl;
	if(totalTableComplete.size() == (unsigned) route.at(0).totalRoutes){

		status = true;
	
	}
	else{
		status = false;
	}

	
	return status;


}

//**************************Manager IP Address*****************************************
char* Manager::getManagerIPAddress(){

	//----------------Get the Host name of the current machine--------------
	char hostname[FixedBufferSize];
	memset(&hostname, '\0', FixedBufferSize);
	gethostname(hostname, sizeof(hostname)); 
	
	//--------------------------------------

	char *serverIp = (char *)malloc(sizeof(char) * 256); //to store server IP address
	struct hostent *server; //The variable server is a pointer to a structure of type hostent
	struct in_addr **addr_list;

	server = gethostbyname(hostname);//get the host info
	if (server == NULL)
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(1);
	}

	addr_list = (struct in_addr **)server->h_addr_list;
	for(int i = 0; addr_list[i] != NULL; i++) {
		serverIp =  inet_ntoa(*addr_list[i]);
	}

	return serverIp;
}



//********************Read the content of the network topology file**************************
void Manager::readTopologyFile(string topologyFile){

	string line = "";
	int lineNum = 0;

	string token;
	string tok;

	routes routeInfo;
	packetInfo packInfo;

	ifstream inputFile(topologyFile);//open topologyFile.txt file
	if(inputFile.fail()){
		cerr <<"Error: Failed to open input file: " << topologyFile <<endl;
		exit(1);
	}
	//-----------------------------	
	//cout<<""<<dateTime <<"[Manager]: reading input file..."<<endl;
	writeToManagerFile("[Manager]: reading input file...");
	
	//---------------------------
	while(!inputFile.eof()){//make sure it is not the end of the file
		while(getline(inputFile, line)){ //goes through each line in the file		

			if(lineNum == 0){ //get the total number of routes
				routeInfo.totalRoutes = atoi(line.c_str());		
				lineNum++;
			}
			else{
				if(line != "-1"){ //get the nodes(nodeAddress(X), NextHope(Y) &Cost(C) for each route)
					routeInfo.vecOfNodesInfo.push_back(line);

					istringstream iss(line);

					while(getline(iss, tok, ' ')){ 
						//----------create the source route as src, dest, cost-----
						//cout<<"*********tok x: "<<tok<<endl;
						routeInfo.nodeAddress = atoi(tok.c_str());
						int tempNodeAddress = atoi(tok.c_str());
						//--------------------
						getline(iss, tok, ' ');
						//cout<<"*********tok y: "<<tok<<endl;
						routeInfo.nextHop = atoi(tok.c_str());
						int tempNextHop = atoi(tok.c_str());
						//-------------------
						getline(iss, tok, ' ');
						//cout<<"*********tok c: "<<tok<<endl;
						routeInfo.cost = atoi(tok.c_str());
						int tempCost = atoi(tok.c_str());
						//--------------

						route.push_back(routeInfo);

						//----------create the destination route as dest, src, cost----
						//cout<<"**************routeInfo.nextHop: "<<routeInfo.nextHop<<endl;
						routeInfo.nodeAddress = tempNextHop;
						routeInfo.nextHop = tempNodeAddress;
						routeInfo.cost = tempCost;
						
						route.push_back(routeInfo);
					
					}

					lineNum++;
				}
				else{
					if(line == "-1"){ //get the source and destination pair of the packet
						while(getline(inputFile, line)){
							if(line != "-1"){
								istringstream iss(line);
								while(getline(iss, token, ' ')){ 
									//------------------
									packInfo.src = atoi(token.c_str());
									//------------------
									getline(iss, token);
									packInfo.dst = atoi(token.c_str());
									//-----------------
									packet.push_back(packInfo);	
									
								}
								
							}
						
						}
					}

					break;
				}

			}

		}
		
		inputFile.close();

	}


	
	totalRoutes =lineNum;
//cout<<"******************totalRoutes: "<<totalRoutes<<endl;
	
	/*cout<<"+++++++++++++Total Number of routes: "<<route.at(0).totalRoutes<<endl;

	//------------------------------
	for(int unsigned i=0; i<route.size(); i++){
		cout<<"*************nodeAddress["<<i<<"]: "<<route.at(i).nodeAddress<<endl;
		cout<<"**********nextHop["<<i<<"]: "<<route.at(i).nextHop<<endl;
		cout<<"*************cost["<<i<<"]: "<<route.at(i).cost<<endl;
		cout<<"----------------------"<<endl;
	}
	cout<<"----------------------"<<endl;

	//---------------------------------
	for(int unsigned i=0; i<packet.size(); i++){
		cout<<"******************packet src["<<i<<"]: "<<packet.at(i).src<<endl;
		cout<<"******************packet dst["<<i<<"]: "<<packet.at(i).dst<<endl;
		cout<<"--------------------"<<endl;

	}
	cout<<"----------------------"<<endl;*/

	//------------------------------
	/*for(int unsigned i=0; i<routeInfo.vecOfNodesInfo.size(); i++){
		cout<<"****************vecOfNodesInfo["<< i<<"]: "<<routeInfo.vecOfNodesInfo.at(i)<<endl;
	}	*/


}

//****************spawn one Unix process for each router in the network*****************************
void Manager::createNetwork(){
	//src: http://www.cs.ecu.edu/karl/4630/spr01/example1.html
	char* managerIP = getManagerIPAddress();

	//cout<<"**************IP: "<<managerIP<<endl;

	int nodeAddress;
	int udpPort = 11000;
	int tcpPort = DEFAUL_TCP_PORT;
	
	char buffer[225];
	char tcpBuffer[225];
	char nodeBuffer[225];
	char totalRoutersBuffer[255];
	//--------		
	char * argv[MAX_ARGS]; //the maximum number of argument the router is going to take. the last one has to be a NULL pointer
	argv[0] = strdup("router");
	//----------
	pid_t child_pid;
	//int child_status;
	//----------
	//cout<<""<<dateTime<<"[Manager]: forking unix process per router..."<<endl;
	//writeToManagerFile("[Manager]: forking unix process per router...");

	//---------------make sure you don't create the same router twice -------------------


	//cout<<"************************route.size(): "<<route.size()<<endl;
	for(int unsigned i=0; i<route.size(); i++){
		tempNodeAddress = route.at(i).nodeAddress;

		if(find(vecNodeAddress.begin(), vecNodeAddress.end(), tempNodeAddress) == vecNodeAddress.end()){
			vecNodeAddress.push_back(tempNodeAddress);
			
		}

	}

	/*for(int unsigned i=0; i<vecNodeAddress.size(); i++){
		cout<<"****************vecNodeAddress: "<<vecNodeAddress.at(i)<<endl;
	}*/
	//----------------------------------------------------------------

	//---------
	//cout<<"***************route.at(0).totalRoutes: "<<route.at(0).totalRoutes<<endl;
	//cout<<"************************totalRoutes: "<<totalRoutes<<endl;
	for(int i =0; i <route.at(0).totalRoutes; i++){
		//-------------------------
		//nodeAddress = route.at(i).nodeAddress;
		nodeAddress = vecNodeAddress.at(i);
		//cout<<"*******************nodeAddress: "<<nodeAddress<<endl;

		//---------creat the child process-------
		child_pid = fork();

		//---------
		if(child_pid == 0){//------this is done by the child process-------- 
			//cout<<"***********child*******"<<endl;
			//----------clear the buffer----------
			memset(buffer, '\0', sizeof(buffer));

			//---------store the udpPort number into the buffer--------
        		sprintf(buffer, "%d", udpPort);

			//---------prepare the argument to be sent to the router to execute
			argv[1] = buffer;


			//----------clear the buffer----------
			memset(tcpBuffer, '\0', sizeof(buffer));

			//---------store the tcpPort number into the buffer--------
        		sprintf(tcpBuffer, "%d", tcpPort);

			//---------continue prepare the argument to be sent to the router to execute
			argv[2] = tcpBuffer;
			
			//----------clear the buffer----------
			memset(nodeBuffer, '\0', sizeof(buffer));

			//---------store the tcpPort number into the buffer--------
        		sprintf(nodeBuffer, "%d", nodeAddress);
			argv[3] = nodeBuffer;

			argv[4] = managerIP;

			//----------clear the buffer----------
			memset(totalRoutersBuffer, '\0', sizeof(buffer));

			//---------store the tcpPort number into the buffer--------
        		sprintf(totalRoutersBuffer, "%d", route.at(0).totalRoutes);
			
			argv[5] = totalRoutersBuffer;
	
			argv[6] = NULL; //NULL pointer to mark the end of the argument--------	
		
			//--------execute the router command-----------

			execv(argv[0], argv); //call the router function and pass in the arguments (updPort,tcpPort,ManagerIP)

			//----If execv returns, it failed to run the command in the terminal
			cerr<<"Unknown command (Unable to create network)"<<endl;
			exit(EXIT_FAILURE);
		}
		else if(child_pid > 0){
			//------this is run by the parent. wait for the chlid to terminate
			//cout<<"***********parent*******"<<endl;
			
			//wait(&child_status);
			
			/*if(WIFEXITED(child_status)){
				cout<<"Exit status: "<<WEXITSTATUS(child_status)<<endl;
			}*/
			

		}
		else{
			cerr<<"fork() failed"<<endl;
			exit(EXIT_FAILURE);
				
		}

		udpPort += 1000;
	}



}

//******************************
string Manager::getRouterInfo(int nodeAddress) {
	string temp;
	stringstream info;

	//cout<<"*********************size: "<<sizeof(receivedUDP)/sizeof(receivedUDP[0])<<endl;
	/*for(int unsigned i=0; i<sizeof(receivedUDP)/sizeof(receivedUDP[0]); i++){
		//cout<<"++++++++++++++++++++++++++++receivedUDP: "<< receivedUDP[i]<<endl;
	}*/

	//cout<<"++++++++++++++++++++++++++++Port: "<< receivedUDP[route[0].nextHop]<<endl;

	//cout<<"*********************size: "<<route.size()<<endl;
	for(int unsigned i=0; i<route.size(); i++){
		
		//cout<<"++++++++++++++++++++++++++++NodeNo: "<<NodeNo<<endl;
		//cout<<"++++++++++++++++++++++++++++route: "<< route.at(i).nodeAddress<<endl;
		if( route.at(i).nodeAddress == nodeAddress){ 
			//cout<<"****************************route: "<< route.at(i).nodeAddress<<endl;
			//temp = "Address: " + to_string(route.at(i).nodeAddress) + "| " + "Neighbor Address: " + to_string(route.at(i).nextHop) + "| " + "Cost to Neighbor: " + to_string(route.at(i).cost) + "| ";
			temp = to_string(route.at(i).nodeAddress) + " " + to_string(route.at(i).nextHop) + " " + to_string(route.at(i).cost) + " " + to_string(receivedUDP[route[i].nextHop]) + " |";
			//cout<<"*********************************temp: "<<temp<<endl;
			info<<temp;
			//cout<<"******************info: "<<info.str()<<endl;
		}

	}
	//cout<<"-------------------------------------"<<endl;
	//cout<<"*********temp: "<<temp<<endl;

	string returnInfo = info.str();
	//cout<<"**************returnInfo: "<<returnInfo<<endl;

	return returnInfo;
}



/********************************************************** 
the router must be told several pieces of information: It's own address, who its neighbors are,  what are the link costs to each neighbor and the UDP port number for each neighbor	
***********************************************************/

void Manager::sendToRouter(){
	//--------
 
	string temp ="";
	//string infoTosend = "";
	char messageHolder[255];
	
	//------------------------------
	
	if(isRoutersConnected()){
		//-------------------------------
		writeToManagerFile("[Manager]: All routers are connected!");
		//--------------------------

		for(int i=0; i<route.at(0).totalRoutes; i++){

		        temp = getRouterInfo(i);
			//cout<<"**********temp: "<<temp<<endl;
			//cout<<"**********tempSize: "<<strlen(temp.c_str())<<endl;
				
		
			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Manager]: 	Sending to router[%d] - |%s...",vecNodeAddress.at(i),temp.c_str());
			writeToManagerFile(messageHolder);

			//------------------

			send(routerTCPsocket[i], temp.c_str(), 255, 0);
			
			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Manager]: 		Router information sent successfully to router [%d]",vecNodeAddress.at(i));
			writeToManagerFile(messageHolder);
				
		
		}



	}

	
}

//***************************Notify routers that it is safe to reach their neighbor************************
void Manager::sendSafeToProceed(){
	char messageHolder[255];

	if(isRoutersReady()){

		bzero(messageHolder, sizeof(messageHolder));
		sprintf(messageHolder, "[Manager]: All routers are ready!");
		writeToManagerFile(messageHolder);
		//--------------

		for(int i=0; i<route.at(0).totalRoutes; i++){
			//-------------
			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Manager]:	Sending to router[%d] - SAFE!",i);
			writeToManagerFile(messageHolder);

			send(routerTCPsocket[i], "SAFE!", 255, 0);

			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Manager]: 		SAFE sent successfully to router [%d]",vecNodeAddress.at(i));
			writeToManagerFile(messageHolder);
		}

	}

}

//********************Notify routers that all links from all routers have been established and the network is up******************
void Manager::sendNetworkIsUP(){
	
	char messageHolder[255];

	if(isRoutersReady()){

		bzero(messageHolder, sizeof(messageHolder));
		sprintf(messageHolder, "[Manager]: All routers links are complete!");
		writeToManagerFile(messageHolder);
		//--------------

		for(int i=0; i<route.at(0).totalRoutes; i++){
			//-------------
			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Manager]:	Sending to router[%d] - NETWORK_IS_UP!",i);
			writeToManagerFile(messageHolder);

			send(routerTCPsocket[i], "NETWORK_IS_UP!", 255, 0);

			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Manager]: 		NETWORK_IS_UP! sent successfully to router [%d]",vecNodeAddress.at(i));
			writeToManagerFile(messageHolder);
		}

	}
}

//***********************************************************8

void Manager::sendSAllBuildPath(){
	char messageHolder[255];

	if(isAllTableComplete()){

		bzero(messageHolder, sizeof(messageHolder));
		sprintf(messageHolder, "[Manager]: All routers Table build complete!");
		writeToManagerFile(messageHolder);
		//--------------

		for(int i=0; i<route.at(0).totalRoutes; i++){
			//-------------
			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Manager]:	Sending to router[%d] - calculate the shortest path tree!",i);
			writeToManagerFile(messageHolder);

			send(routerTCPsocket[i], "LSP!", 255, 0);

			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Manager]: 		calculate the shortest path tree! sent successfully to router [%d]",vecNodeAddress.at(i));
			writeToManagerFile(messageHolder);
		}

	}
}


//********************Notify routers that all routers are done with limited broadcast******************
void Manager::sendSAllRoutersBroadcastComplete(){
	
	char messageHolder[255];

	if(isRoutersLinkComplete()){

		bzero(messageHolder, sizeof(messageHolder));
		sprintf(messageHolder, "[Manager]: All routers limited broacast complete!");
		writeToManagerFile(messageHolder);
		//--------------

		for(int i=0; i<route.at(0).totalRoutes; i++){
			//-------------
			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Manager]:	Sending to router[%d] - ALL_COMPLETE!",i);
			writeToManagerFile(messageHolder);

			send(routerTCPsocket[i], "ALL_COMPLETE!", 255, 0);

			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Manager]: 		ALL_COMPLETE! sent successfully to router [%d]",vecNodeAddress.at(i));
			writeToManagerFile(messageHolder);
		}

	}
}

//****************** The manager communicates with the routers via TCP************************
void Manager::managerProcess(){
	//src: http://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/

	//cout<<""<<dateTime<<"[Manager]: TCP socket is being created..."<<endl;
	writeToManagerFile("[Manager]: TCP socket is being created...");	

	//-----variables-------
	int managerTCPsock;
	int portno = DEFAUL_TCP_PORT;
	int activity, i , sd, max_sd; //used for select()
	int valread;
	char buffer[1025];	

	//----------create set of socket descriptors-----------------
	fd_set readfds;

	//----------initialise all routerTCPsocket[] to 0 so not checked--------------
	for(i = 0; i< MAX_ROUTES; i++){
		routerTCPsocket[i] = 0;
	}

	//---------create a manager socket to listen for request from each routers------------------------
	managerTCPsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (managerTCPsock < 0) {
		fprintf(stderr, "Manager failed to create socket\n");
		exit(1);
	}

	//----------set manager socket to allow multiple connections (Good habit)------------ 
	int opt = 1;
	if(setsockopt(managerTCPsock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) <0){
		fprintf(stderr, "Manager: setsockopt failed\n");
		exit(1);
	}

	//---------type of socket created-----------------
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(portno);

	//-------bind the address of the current host and the port number--------------
	if (bind(managerTCPsock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		//fprintf(stderr, "Manager error on binding\n");
		perror("Manager error on binding: ");
		exit(1);
	}

	//--------Listen for connection----------------------
	if (listen(managerTCPsock, MAX_ROUTES) < 0) {
		fprintf(stderr, "Manager Error Listening\n");
		exit(1);
	}

	//----------------Get the Host name of the current machine--------------
	char hostname[FixedBufferSize];
	memset(&hostname, '\0', FixedBufferSize);
	gethostname(hostname, sizeof(hostname)); 

	
	//---------------Messages to print on the screen for the Manager-------------------

	char* serverIp = getManagerIPAddress();
	char messageHolder[255];
	//-------
	//cout<<""<<dateTime<<"[Manager]: TCP socket succussfully created!"<<endl;
	writeToManagerFile("[Manager]: TCP socket succussfully created!");
	//----------	
	//cout<<""<<dateTime<<"[Manager]: Running on host - " <<hostname<<endl;
	sprintf(messageHolder, "[Manager]: Running on host - %s", hostname);
	writeToManagerFile(messageHolder);
	//----------
	//cout<<""<<dateTime<<"[Manager]: IP Address - " << serverIp<<endl;
	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Manager]: IP Address - %s", serverIp);
	writeToManagerFile(messageHolder);
	//------------
	//cout<<""<<dateTime<<"[Manager]: Listening to routers on port - " <<portno<<endl;
	bzero(messageHolder, sizeof(messageHolder));
	sprintf(messageHolder, "[Manager]: Listening to routers on port - %d", portno);
	writeToManagerFile(messageHolder);
	//-------accept the incoming connection (causes the process to block until a client connects to the server)------------ 

	struct sockaddr_in clientSockAddr;
	
	unsigned int clientAddrSize = sizeof(clientSockAddr);
	

  	while(1)
	{
		//--------clear the socket set-------------
		FD_ZERO(&readfds);
		
		//------add manager socket to set--------------
		FD_SET(managerTCPsock, &readfds);
		max_sd = managerTCPsock;

		//-------add router TCP socket to set-------------
		for(i = 0; i< MAX_ROUTES; i++){
			//--socket descriptor--
			sd = routerTCPsocket[i];

			//--if valid socket descriptor then add to read list--
			if(sd > 0){
				FD_SET(sd , &readfds);
			}

			//--highest file descriptor numner, 
			//--need it for the select function--
			if(sd > max_sd){
				max_sd = sd;
			}
		}

		//--wait for an activity on one of the sockets , timeout is NULL , 
		//--so wait indefinitely 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  

		if ((activity < 0) && (errno!=EINTR))  
		{  
		   cout<<"Manager select error"<<endl;
		}  
		    
		//--If something happened on the manager socket , 
		//--then its an incoming connection 
		if (FD_ISSET(managerTCPsock, &readfds))  
		{ 
			if ((routerTCPsock = accept(managerTCPsock,(struct sockaddr *) &clientSockAddr, &clientAddrSize))<0){  
				fprintf(stderr, "Manager error on accpet \n");
				exit(1);  
			}  
			
			//send new connection greeting message 

			//send(routerTCPsock, "welcome!", 255, 0);
			//cout<<""<<dateTime<<"[Manager]: welcome message sent successfully"<<endl;
			
			//receive message send from router
			bzero(buffer,sizeof(buffer)); //clear the buffer

			int returnVal = recv(routerTCPsock, buffer, sizeof(buffer), 0);
			if(returnVal < 0){
				perror("Manager Error reading from router");
				exit(1);
			}

			//----------------------
			//cout<<""<<dateTime<<"[Manager]: received from router Port- "<<buffer<<endl;                

			bzero(messageHolder, sizeof(messageHolder));
			sprintf(messageHolder, "[Manager]: Received from %s", buffer);
			writeToManagerFile(messageHolder);

			//--------extract the UDP Port from the message------------

			//cout<<"***************buffer: "<<buffer<<endl;
			string t =buffer;
			string UDPstr = t.substr(t.find("-")+2);

			char UDPbuff[255]; 
			bzero(UDPbuff, sizeof(UDPbuff));
			sprintf(UDPbuff, "%s", UDPstr.c_str());
			//cout<<"**************t: "<<t.substr(t.find("-")+2)<<endl;

			receivedUDP_Message.push_back(atoi(UDPbuff));
			
			//-------Extract the router number----------
			int startPos = t.find("[")+1;
			string routerNumRecieved = t.substr(startPos, t.find("]")-startPos);
			//cout<<"***********routerNumRecieved: "<<routerNumRecieved<<endl;
			
			receivedRouterNum.push_back(atoi(routerNumRecieved.c_str()));
			
			//add new socket to array of sockets 
			for (i = 0; i < route.at(0).totalRoutes; i++)  
			{  
				//cout<<"************receivedRouterNum: "<<receivedRouterNum.at(i)<<endl;
				//if position is empty 
				if( routerTCPsocket[i] == 0 )  
				{  
					routerTCPsocket[atoi(routerNumRecieved.c_str())] = routerTCPsock;  
					receivedUDP[atoi(routerNumRecieved.c_str())] =  atoi(UDPbuff);
					break;  
				}  
			}  
			
			//send node address and connectivity table to routers
			sendToRouter();
			//send(routerTCPsocket[0], "+++++++++welcome!", 255, 0);

		}

		//else its some IO operation on some other socket
		for(i=0; i<MAX_ROUTES; i++)
		{
			sd = routerTCPsocket[i];
			
				if(FD_ISSET( sd , &readfds)){
					//---------------Receive message from the router----------------------
					bzero(buffer, sizeof(buffer));
					//Check if it was for closing , and also read the incoming message 
					if ((valread = recv( sd , buffer, sizeof(buffer) , 0))< 0)  
					{  
						cout<<"Error Manager unable to read from router\n";
					}  
					
					//--------------Case where message receive is equal to READY!-----------------
					if(strcmp(buffer, "READY!") == 0){
						bzero(messageHolder, sizeof(messageHolder));
						sprintf(messageHolder, "[Manager]: Received from router[%d] - Ready to accept link establishment requests from neighbor nodes", i);
						writeToManagerFile(messageHolder);

						//---------
						totalReady.push_back(i);
						//------
						//sendSafeToProceed();
						sendNetworkIsUP();

					}
		
					//--------------Case where message receive is equal to COMPLETE!-----------------
					if(strcmp(buffer, "COMPLETE!") == 0){
						bzero(messageHolder, sizeof(messageHolder));
						sprintf(messageHolder, "[Manager]: Received from router[%d] - Limited Broadcast COMPLETE!", i);
						writeToManagerFile(messageHolder);
//cout<<"********************Router: "<<i<<endl;
						//---------
						totalLinkComplete.push_back(i);
						//------
						sendSAllRoutersBroadcastComplete();
						

					}
					
					if(strcmp(buffer, "TABLE_READY!") == 0){
						bzero(messageHolder, sizeof(messageHolder));
						sprintf(messageHolder, "[Manager]: Received from router[%d] - Ready to start shortest path table", i);
						writeToManagerFile(messageHolder);


						//cout<<"*************Router["<<i<<"]: "<<"Ready to start shortest path table"<<endl;
						//---------
						totalTableComplete.push_back(i);
						//------
						sendSAllBuildPath();
						

					}
					
					
				}
		}
		
		

	}





}




//********************************************************************************************************
int main(int argc, char *argv[]) {

	string topologyFile = ""; //contains the network topology description
	Manager manager;


	//------------------Make sure the user enter the proper arguments------------------------------
	if(argc == 2){
		topologyFile = argv[1];
	}
	else{
		cout<<"Usage: manager <input file>"<<endl;
		return 1;
	}
	//--------clear the manager file before writing the new output
	manager.managerFile.open("manager.out", std::ofstream::out | std::ofstream::trunc);
	manager.managerFile.close();	
	//------------- 
	dateTime = manager.currentDateTime(); //get the current date and time
	//------------

	cout<<dateTime <<"[Manager]: Process has started"<<endl;
	cout<<"	"<<dateTime <<"check manager.out for process status\n"<<endl;

	cout<<dateTime<<"[Router]: Process has started"<<endl;
	cout<<"	"<<dateTime <<"check <routerID>.out for process status\n"<<endl;

	manager.writeToManagerFile("[Manager]: Process has started\n");

	//-----------------
	
	manager.readTopologyFile(topologyFile);


	//------Make sure the Manger is listining for connection before the router is created----------------
	thread managerProcess_thread = manager.managerProcessThread();
	thread createNetwork_thread = manager.createNetworkThread();
	managerProcess_thread.join();
	createNetwork_thread.join();
	
	//--------------------
	

	return 0;
}

