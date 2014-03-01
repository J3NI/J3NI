#ifndef CHANNELCMD_H
#define CHANNELCMD_H

#include <I_Command.h>

class  GetChannelAuthCMD:public I_Command{
private:
    static const unsigned char channelNum = 0x01;
public:
    unsigned char getChannelNum();
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

#endif