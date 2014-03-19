//
//  DaemonServer.h
//
//  Copyright (c) 2014 J3NI. All rights reserved.
//

#ifndef DAEMONSERVER_H
#define DAEMONSERVER_H

#include <netdb.h>

class IpmiMessage;

class DaemonServer {
private:
    
    int port;
    pid_t pid, sid;
    
    struct sockaddr_in localAddr;   // Local address
    struct sockaddr_in remoteAddr;  // Client address
    
    int sock;                       //Socket
    
    static const int BUF_SIZE;

    
public:
    
   DaemonServer();
   DaemonServer(int port);
   
   ~DaemonServer(){};
    
   void startDaemon();
   void startServer();
   void receiveData();
    
private:
    void logMessage(const IpmiMessage& msg);
    bool validateMessage(const IpmiMessage& msg);

};

#endif
