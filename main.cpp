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

// Local Includes
#include "DaemonServer.h"

// Setup local log file
std::ofstream log_file(
                       "J3NI_log_file.log", std::ios_base::out | std::ios_base::app );

int main(int args, char** argv)
{
    DaemonServer udpDaemon(8087);
    udpDaemon.startDaemon();
    
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

