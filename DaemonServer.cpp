//
//  DaemonServer.cpp
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

#include "DaemonServer.h"

extern std::ofstream log_file;

const int DaemonServer::BUF_SIZE = 2048;

DaemonServer::DaemonServer()
    : port(-1), username(NULL), password(NULL)
{
}

DaemonServer::DaemonServer(int port)
    : port(port), username(NULL), password(NULL)
{
}

DaemonServer::DaemonServer(int port, char* uname, char* pass)
    : port(port), username(uname), password(pass)
{
}


void DaemonServer::startDaemon()
{
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
    
    pid = pid;
    sid = sid;
    
    // Set File Permissions
    umask(0); // 027 may be better?
    
    // Run in root directory
    chdir("/");
    
    log_file << "Daemon started successfully" << std::endl;
}


void DaemonServer::startServer()
{
    /* create a UDP socket */
    
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
		perror("cannot create socket\n");
	}
    
	/* bind the socket to any valid IP address and a specific port */
    
	memset((char *)&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(port);
    
	if (bind(sock, (struct sockaddr *)&localAddr, sizeof(localAddr)) < 0)
    {
		perror("bind failed");
	}
}

void DaemonServer::receiveData()
{
    long int recvLen = 0;
    int msgCount = 0;
    char* buf = new char[BUF_SIZE];
    
    socklen_t addrlen = sizeof(remoteAddr);
    
    recvLen = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&remoteAddr, &addrlen);
    if (recvLen > 0)
    {
        buf[recvLen] = 0;
        log_file << "Received a message" << std::endl;
    }
    else
    {
        log_file << "Something went wrong\n" << std::endl;
    }
    delete [] buf;
}
