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
#include <netinet/in.h>

#include <fstream>
//#include <tr1/array>

#include <DaemonServer.h>
#include <MsgHandler.h>

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
    
    log_file << "Daemon started successfully\n";
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
    long int respLen = 0;

    unsigned char* buf = new unsigned char[BUF_SIZE];
    //std::tr1::array<unsigned char, BUF_SIZE>* buf;
    unsigned char* response = new unsigned char[BUF_SIZE];

    socklen_t addrlen = sizeof(remoteAddr);
    
    recvLen = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&remoteAddr, &addrlen);

    if (recvLen > 0)
    {
        // Print received message to log
        log_file << "Received a message of size "<< recvLen << std::endl;
        for (int i = 0; i < recvLen; i++)
            log_file << std::hex << (int)buf[i]<< " | " << std::flush;  
        log_file << std::dec <<  std::endl;
        
        // Determine appropriate response
        if (recvLen == 12 && MsgHandler::is_ping(buf)) 
            respLen = MsgHandler::pong(buf, response);
        else
            respLen = MsgHandler::processRequest(buf, response);
      
        // Send response
        if (sendto(sock, response, respLen , 0, (struct sockaddr *)&remoteAddr, addrlen) < 0)
            log_file << "Error sending response" << std::endl;
        else
            log_file << "Sent a message of size "<< respLen << std::endl;

        // Print sent message to log 
        for (int i = 0; i < respLen; i++)
            log_file << std::hex << (int)response[i]<< " | " << std::flush;
        log_file << std::dec <<  std::endl;
    }
    else
    {
        log_file << "Could not receive request." << std::endl;
    }
    
    // Empty request buffer
    delete [] buf;
    
}
