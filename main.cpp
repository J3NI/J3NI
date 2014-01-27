//
//  main.cpp
//
//  Copyright (c) 2014 J3NI. All rights reserved.
//

// System Includes
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <fstream>
#include <iostream>
#include <cstring>

// Local Includes
#include "DaemonServer.h"

// Setup local log file
std::ofstream log_file(
                       "J3NI_log_file.log", std::ios_base::out | std::ios_base::app );

int main(int args, char** argv)
{

    int portnum=8087;
    char* user = NULL;
    char* password = NULL;

    if(args==1){ 
    }
    else if(args==3 && (strcmp(argv[1],"-p")==0)){
	    portnum = atoi(argv[2]);
	    if(portnum==0){
		    std::cout<< "Invalid input entered" << std::endl;
		    exit(EXIT_FAILURE);
	    }
    }
    else if(args==7){
	    bool portSet = false;
	    bool userSet = false;
	    bool passSet = false;
	   for(int i=1; i<6; i+=2){
		    if(strcmp(argv[i],"-p")==0 && !portSet){
			    portnum=atoi(argv[i+1]);
			    if(portnum==0){
				    std::cout<< "Invalid input entered" << std::endl;
				    exit(EXIT_FAILURE);
			    }		
			    portSet=true;
		    }
		    if(strcmp(argv[i],"-u")==0 && !userSet){
			    user=argv[i+1];
			    userSet=true;
		    }
		    if(strcmp(argv[i],"-k")==0 && !passSet){
			    password=argv[i+1];
			    passSet=true;
		    }		 
	    }
	    if(!portSet || !userSet || !passSet){	// If not all fields are set, exit
		    std::cout<< "Invalid input entered" << std::endl;
		    exit(EXIT_FAILURE);
	    }
    }
    else{
	    std::cout<< "Invalid input entered" << std::endl;
	    exit(EXIT_FAILURE);
    }
    
    DaemonServer* udp_daemon = new J3NI_Daemon(portnum, user, password);

    udp_daemon->startDaemon();
    
    //! \todo   Signal Handling
    
    
    openlog("IpmiDaemon", LOG_PID, LOG_DAEMON);
    syslog(LOG_NOTICE, "Daemon Started");
    
    udpDaemon.startServer();
    
    while(1)
    {
        udpDaemon.receiveData();
        
        //! \todo   remaining implementation
        sleep(2);
    }
    
    return 1;
}

