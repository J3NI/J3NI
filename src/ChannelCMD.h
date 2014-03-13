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

class GetChannelInfoCMD:public I_Command{
private:
    GetChannelAuthCMD* getAuthCMD;
public:
    GetChannelInfoCMD(GetChannelAuthCMD* getAuth);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class SetChannelAccessCMD:public I_Command{
private:
    // Default 0x2f
    // [5] - Alerting disabled
    // [4] - Per message authentication disabled
    // [3] - User level authentication disabled
    // [2:0] - Channel always available
    unsigned char accessSettings;
    
    // Default ADMINISTRATOR limit
    unsigned char privLvl;
public:
    SetChannelAccessCMD();
    unsigned char getPrivLvl();
    unsigned char getSettings();    
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class GetChannelAccessCMD:public I_Command{
private:
    SetChannelAccessCMD* chanAccCMD;
public:
    GetChannelAccessCMD(SetChannelAccessCMD* chanAcc);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};
#endif
