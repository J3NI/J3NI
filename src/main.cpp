//
//  main.cpp
//
//  Copyright (c) 2014 J3NI. All rights reserved.
//

// System Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <cstring>

// Local Includes
#include <DaemonServer.h>

using namespace std;

// Setup local log file
ofstream log_file("J3NI_log_file.log", ios_base::out | ios_base::app );

int main(int args, char** argv)
{

    int portnum=8087;
    char* user = NULL;
    char* password = NULL;
   
   DaemonServer* udpDaemon = NULL;

    if(args==1){
    }
    else if(args==3 && (strcmp(argv[1],"-p")==0))
    {
	    portnum = atoi(argv[2]);
	    if(portnum==0)
       {
		    cout<< "Invalid input entered\n";
		    exit(EXIT_FAILURE);
	    }
    }
    else if(args==7){
       bool portSet = false;
       for(int i=1; i<6; i+=2){
		    if(strcmp(argv[i],"-p")==0 && !portSet){
			    portnum=atoi(argv[i+1]);
			    if(portnum==0){
				    cout<< "Invalid input entered\n";
				    exit(EXIT_FAILURE);
			    }		
			    portSet=true;
		    }
		    if(strcmp(argv[i],"-u")==0 && !user){
			    user=argv[i+1];
		    }
		    if(strcmp(argv[i],"-k")==0 && !password){
			    password=argv[i+1];
		    }		 
	    }
	    if(!portSet || !user || !password){	// If not all fields are set, exit
          cout<< "Invalid input entered\n";
		    exit(EXIT_FAILURE);
	    }
    }
    else{
	    cout<< "Invalid input entered\n";
	    exit(EXIT_FAILURE);
    }
    
    udpDaemon = new DaemonServer(portnum, user, password);

    udpDaemon->startDaemon();
    
    udpDaemon->startServer();
    
    while(1)
    {
        udpDaemon->receiveData();
        sleep(2);
    }
   
    delete udpDaemon;
    return 0;
}

