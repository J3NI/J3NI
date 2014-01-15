//
//  J3NI_Daemon.h
//
//  Copyright (c) 2014 J3NI. All rights reserved.
//

#ifndef J3NI_DAEMON_H
#define J3NI_DAEMON_H

#define BUFSIZE 2048
extern std::ofstream log_file;

class J3NI_Daemon {
	private:
    
    int port;
    char* username;
    char* password;
    pid_t pid, sid;
    
    struct sockaddr_in local_addr; // Local address
    struct sockaddr_in remote_addr; // Client address
    
    int sock;				//Socket

	
	public:
    J3NI_Daemon() {}; // Default constructor
    J3NI_Daemon(int port): port(port), username(""), password("") {}; // Anonymous user constructor
    J3NI_Daemon(int port, char* uname, char* pass): port(port), username(uname), password(pass) {}; // Full auth constructor
        
    void startDaemon();
    void startServer();
    void receiveData();

};

#endif
