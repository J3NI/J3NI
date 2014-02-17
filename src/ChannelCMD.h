#ifndef CHANNELCMD_H
#define CHANNELCMD_H

class  GetChannelAuthCMD:public cmdProcessor{
public:
    int process( const unsigned char* request, unsigned char* response );
};

#endif