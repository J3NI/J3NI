//
//  J3NI_Daemon.cpp
//
//  Copyright (c) 2014 J3NI. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <fstream>

#include "J3NI_Daemon.h"

void J3NI_Daemon::startDaemon() {
    pid_t pid, sid;
    
    // Fork processes
    pid = fork();
    if(pid < 0)
    {
        printf("Fork process failed\n");
        exit(EXIT_FAILURE);
    }
    else if(pid > 0)
    {
        exit(EXIT_SUCCESS);
    }
    
    // Session ID
    sid = setsid();
    if(sid < 0)
    {
        exit(EXIT_FAILURE);
    }
    
    // Fork again so the child cannot be associated with a terminal
    pid = fork();
    if(pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    else if(pid > 0)
    {
        exit(EXIT_SUCCESS);
    }
    
    this->pid = pid;
    this->sid = sid;
    
    // Set File Permissions
    umask(0); // 027 may be better?
    
    // Run in root directory
    chdir("/");
    
    log_file << "Daemon started successfully" << std::endl;
}


void J3NI_Daemon::startServer(){
    /* create a UDP socket */
    
	if ((this->sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
	}
    
	/* bind the socket to any valid IP address and a specific port */
    
	memset((char *)&this->local_addr, 0, sizeof(this->local_addr));
	this->local_addr.sin_family = AF_INET;
	this->local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	this->local_addr.sin_port = htons(this->port);
    
	if (bind(this->sock, (struct sockaddr *)&this->local_addr, sizeof(this->local_addr)) < 0) {
		perror("bind failed");
	}
}

void J3NI_Daemon::receiveData(){
    int recvlen;
    int msgcnt = 0;
    char buf[BUFSIZE];
    
    	socklen_t addrlen = sizeof(this->remote_addr);	
    
    recvlen = recvfrom(this->sock, buf, BUFSIZE, 0, (struct sockaddr *)&this->remote_addr, &addrlen);
    if (recvlen > 0) {
        buf[recvlen] = 0;
        //printf("received message: \"%s\" (%d bytes)\n", buf, recvlen);
        log_file << "Received a message" << std::endl;
    }
    else
        log_file << "Something went wrong\n" << std::endl;
}
