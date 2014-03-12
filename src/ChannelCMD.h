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

class GetChannelCipherSuitesCMD:public I_Command{
private:
    GetChannelAuthCMD* GetChanAuthCMD;
public:
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};


class GetChannelInfoCMD:public I_Command{
private:
    GetChannelAuthCMD* GetChanAuthCMD;
public:
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class SetChannelAccessCMD:public I_Command{
private:
    unsigned char chanPrivLvl;
public:
    unsigned char getChanPrivLvl();
    int setChanPrivLvl( const unsigned char newLvl );
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class GetChannelAccessCMD:public I_Command{
private:
    SetChannelAccessCMD* chanAccCMD;
public:
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};
#endif
