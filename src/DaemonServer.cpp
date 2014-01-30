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
#include <iomanip>

#include <DaemonServer.h>
#include <MsgHandler.h>
#include <IpmiMessage.h>

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
    int recvLen = 0;

    unsigned char* buf = new unsigned char[BUF_SIZE];

    socklen_t addrlen = sizeof(remoteAddr);
    
    recvLen = (int)recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&remoteAddr, &addrlen);

    if (recvLen > 0)
    {
        // Print received message to log
        IpmiMessage recievedMsg(buf, recvLen);
        log_file << "Received a message of size "<< recievedMsg.length() << std::endl;
        logMessage(recievedMsg);
        
        // Determine appropriate response
        IpmiMessage response;
        if (MsgHandler::isPing(recievedMsg))
            MsgHandler::pong(recievedMsg, response);
        else
            MsgHandler::processRequest(recievedMsg, response);
      
        // Send response
        if (sendto(sock, response.message(), response.length(), 0, (struct sockaddr *)&remoteAddr, addrlen) < 0)
            log_file << "Error sending response\n";
        else
            // Print sent message to log
            log_file << "Sent a message of size "<< response.length() << std::endl;
        
        logMessage(response);
    }
    else
    {
        log_file << "Could not receive request.\n";
    }
    
    // Empty request buffer
    delete [] buf;
    
}

void DaemonServer::logMessage(const IpmiMessage& msg)
{
    log_file << std::hex << std::setfill('0');
    for (int i = 0; i < msg.length(); i++)
        log_file << std::setw(2) << msg[i] << " | ";
    log_file << std::dec << std::setfill(' ') <<  std::endl << std::flush;
}

