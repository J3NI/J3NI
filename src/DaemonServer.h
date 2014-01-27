//
//  DaemonServer.h
//
//  Copyright (c) 2014 J3NI. All rights reserved.
//

#ifndef DAEMON_SERVER_H
#define DAEMON_SERVER_H

#include <netdb.h>

class DaemonServer {
	private:
    
    int port;
    char* username;
    char* password;
    pid_t pid, sid;
    
    struct sockaddr_in localAddr;   // Local address
    struct sockaddr_in remoteAddr;  // Client address
    
    int sock;                       //Socket
    
    static const int BUF_SIZE;

	
	public:
    
    DaemonServer();
    DaemonServer(int port);
    DaemonServer(int port, char* uname, char* pass);
    
    void startDaemon();
    void startServer();
    void receiveData();

};

#endif
