#include "manager.h"


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
	
	cout<<"Manager reading input file..."<<endl;

	while(!inputFile.eof()){//make sure it is not the end of the file
		while(getline(inputFile, line)){ //goes through each line in the file		

			if(lineNum == 0){ //get the total number of routes
				routeInfo.totalRoutes = atoi(line.c_str());		
				lineNum++;
			}
			else{
				if(line != "-1"){ //get the nodes(destination(X), NextHope(Y) &Cost(C) for each route)
					routeInfo.vecOfNodesInfo.push_back(line);

					istringstream iss(line);

					while(getline(iss, tok, ' ')){ 
						//-----------------
						//cout<<"*********tok x: "<<tok<<endl;
						routeInfo.dest = atoi(tok.c_str());
						
						//--------------------
						getline(iss, tok, ' ');
						//cout<<"*********tok y: "<<tok<<endl;
						routeInfo.nextHop = atoi(tok.c_str());
						
						//-------------------
						getline(iss, tok, ' ');
						//cout<<"*********tok c: "<<tok<<endl;
						routeInfo.cost = atoi(tok.c_str());
						//--------------
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


	

	
	cout<<"+++++++++++++Total Number of routes: "<<route.at(0).totalRoutes<<endl;

	//------------------------------
	for(int unsigned i=0; i<route.size(); i++){
		cout<<"*************dest["<<i<<"]: "<<route.at(i).dest<<endl;
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
	cout<<"----------------------"<<endl;

	//------------------------------
	/*for(int unsigned i=0; i<routeInfo.vecOfNodesInfo.size(); i++){
		cout<<"****************vecOfNodesInfo["<< i<<"]: "<<routeInfo.vecOfNodesInfo.at(i)<<endl;
	}	*/


}

//***************************************************************************************
void Manager::createNetwork(){
	
	int udpPort = 11000;
	char buffer[225];
	//--------		
	char * argv[MAX_ARGS]; //the maximum number of argument the router is going to take. the last one has to be a NULL pointer
	argv[0] = strdup("router");
	//----------
	pid_t child_pid;
	int child_status;
	//----------
	cout<<"Manager has started forking unix process per router..."<<endl;
	//---------

	for(int i =0; i <route.at(0).totalRoutes; i++){

		//---------creat the child process-------
		child_pid = fork();

		//---------
		if(child_pid == 0){//------this is done by the child process-------- 
			//cout<<"************child*******"<<endl;
			//----------clear the buffer----------
			memset(buffer, '\0', sizeof(buffer));

			//---------store the udpPort number into the buffer--------
        		sprintf(buffer, "%d", udpPort);
//cout<<"****************Port: "<<udpPort<<endl;
			//---------prepare the argument to be sent to the router to execute(ex: router 11000)--
			argv[1] = buffer;
			argv[2] = NULL; //NULL pointer to mark the end of the argument--------	
		
			//--------execute the router command-----------
			//execv(argv[0], NULL);
			execv(argv[0], argv); //call the router function and pass in the arguments

			//----If execv returns, it failed to run the command in the terminal
			cerr<<"Unknown command (Unable to create network)"<<endl;
			exit(EXIT_FAILURE);
		}
		else if(child_pid > 0){
			//------this is run by the parent. wait for the chlid to terminate
			//cout<<"***********parent*******"<<endl;
			
			wait(&child_status);
	
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

	
	





//********************************************************************************************************
int main(int argc, char *argv[]) {

	string topologyFile = ""; //contains the network topology description



	//------------------Make sure the user enter the proper arguments------------------------------
	if(argc == 2){
		topologyFile = argv[1];
	}
	else{
		cout<<"Usage: manager <input file>"<<endl;
		return 1;
	}

	//-----------------
	Manager manager;
	manager.readTopologyFile(topologyFile);

	//-------------
	manager.createNetwork();



	return 0;
}
