//
//  main.cpp
//  J3NI
//
//  Created by Neil on 2014-01-12.
//  Copyright (c) 2014 Neil. All rights reserved.
//

// System Includes
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Local Includes

void startDaemon();

void startDaemon()
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
    
    // Set File Permissions
    umask(0);
    
    // Run in root directory
    chdir("/");
    
}

int main(int args, char** argv)
{
    startDaemon();
    
    //! \todo   Signal Handling
    
    // Setup Log File
    openlog("IpmiDaemon", LOG_PID, LOG_DAEMON);
    syslog(LOG_NOTICE, "Daemon Started");
    
    while(1)
    {
        //! \todo   remaining implementation
        sleep(30);
    }
}
