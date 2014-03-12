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
    char* username;
    char* password;
    pid_t pid, sid;
    
    struct sockaddr_in localAddr;   // Local address
    struct sockaddr_in remoteAddr;  // Client address
    
    int sock;                       //Socket
    
    static const int BUF_SIZE;
    
    bool sessionEstablished;
    uint32_t sessionId_;
    uint32_t sequenceNumber_;
    
public:
    
   DaemonServer();
   DaemonServer(int port);
   DaemonServer(int port, char* uname, char* pass);
   
   ~DaemonServer(){};
    
   void startDaemon();
   void startServer();
   void receiveData();
    
private:
    void logMessage(const IpmiMessage& msg);
    bool validateMessage(const IpmiMessage& msg);

};

#endif
