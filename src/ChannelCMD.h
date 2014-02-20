#ifndef CHANNELCMD_H
#define CHANNELCMD_H

#include <I_Command.h>

class  GetChannelAuthCMD:public I_Command{
public:
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

#endif