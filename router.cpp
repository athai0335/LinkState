#include "router.h"
#include "manager.h"


int udpPort;
int tcpPort;
int routerTCPsock;
int routerUDPsock;
string managerIP;
string dateTime;

//******************Use to send message to manager*******************
void Router::sendToManager( char* message){
	cout<<""<<dateTime<<"[Router]: sending to manager - " <<message <<endl;
	int returnVal = send(routerTCPsock,message,strlen(message),0);
	if (returnVal < 0){
		cout<<"Error: Router enable to send message to router"<<endl;
		exit(1);
	}
	cout<<""<<dateTime<<"[Router]: message sent successfully!" <<endl;
}

//***************use to receive message from the manager****************
char* Router::receiveFromManager(){
	cout<<""<<dateTime<<"[Router]: receiving from manager..."<<endl;
	
	char buffer[255];
	bzero(buffer,255); //clear the buffer
	 
	recv(routerTCPsock, buffer, sizeof(buffer), 0);
	
	cout<<""<<dateTime<<"[Router]: received from manager - " << buffer<<endl;
	
	char* str = (char *)malloc(sizeof(char) * 256);
	memset(str, '\0', sizeof(buffer) + 1);
	strncpy(str, buffer, sizeof(buffer));
	return str;
}

//*****************process for each routers***************************
void Router::routerProcess(){
	
	cout<<dateTime<<"[Router]: Process started"<<endl;
	
	//------varaiables---------
	int activity , max_sd; //used for select()
	//int sd;
	char buffer[255];
	//----------create set of socket descriptors-----------------
	fd_set readfds;
	

	//-----------create a TCP socket for the router to communicate with the Manager-------
  	cout<<""<<dateTime<<"[Router]: creating TCP socket..."<<endl;

    	struct sockaddr_in serverTCP_addr;

	routerTCPsock = socket(AF_INET, SOCK_STREAM,0);

	if (routerTCPsock < 0) {
		fprintf(stderr, "Error: Router failed to create TCP socket\n");
		exit(1);
	}

	cout<<""<<dateTime<<"[Router]: TCP socket created."<<endl;
	cout<<""<<dateTime<<"[Router]: TCP Port: "<<tcpPort<<endl;
	
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
	cout<<""<<dateTime<<"[Router]: creating UDP socket..."<<endl;
	
	routerUDPsock = socket(AF_INET, SOCK_DGRAM,0);

	if (routerUDPsock < 0) {
		fprintf(stderr, "Error: Router failed to create UDP socket\n");
		exit(1);
	}

	cout<<""<<dateTime<<"[Router]: UDP socket created."<<endl;
	cout<<""<<dateTime<<"[Router]: UDP Port: "<<udpPort<<endl;

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
	cout<<""<<dateTime<<"[Router]: Connecting to manager <"<<inet_ntoa(serverTCP_addr.sin_addr)<<">..."<< endl;
	//printf("Router connecting To: %s \n", inet_ntoa(serverTCP_addr.sin_addr));

	if (connect(routerTCPsock, (struct sockaddr *) &serverTCP_addr, sizeof(serverTCP_addr)) < 0){
		fprintf(stderr, "Error: Router Unable to connect\n");
		pthread_exit(NULL);
	}

    	cout<<""<<dateTime<<"[Router]: connected succssuflly to manager!"<<endl;
	

	//--------------sending the UDP port to manager---------------
	char udpPortBuffer[255];
	bzero(udpPortBuffer,255); //clear the buffer

        sprintf(udpPortBuffer, "%d", udpPort);
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
	
		//-------add router TCP socket to set-------------

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
		//--then its an incoming connection 
		if (FD_ISSET(routerTCPsock, &readfds))  
		{ 
			//----receive message send from Manager
			receiveFromManager(); 

			//-----send ready message to manager-------      
			bzero(buffer,255); //clear the buffer
      			sprintf(buffer, "%s", "READY");
			sendToManager(buffer);

		}


	}








	//------------
	//receiveFromManager();
	
}








//***************************************************************************************
int main(int argc, char *argv[]) {
	
	cout<<endl;
	cout<<"*********Hello World! from router*********"<<endl;
	//----------	
	Manager manager;
	dateTime = manager.currentDateTime();
	
	//---------------
	udpPort = atoi(argv[1]);
	tcpPort = atoi(argv[2]);
	managerIP = argv[3];

	Router router;
	router.routerProcess();
	
	


	/*for(int i=1; i<argc; i++){
		cout<<"**********argv: "<<argv[i]<<endl;
		//router.routerProcess(argv[i]);
	}*/

	return 0;
}
