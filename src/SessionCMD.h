#ifndef SESSIONCMD_H
#define SESSIONCMD_H

#include <I_Command.h>
class GetChannelAuthCMD;

class GetSessionChalCMD:public I_Command{
private:
    unsigned char* userName_;
    
private:
    unsigned char verifyUserName(const unsigned char* uname) const;
    
public:
    GetSessionChalCMD(const char* userName);
    virtual ~GetSessionChalCMD();
    
    int process(const unsigned char* request, int reqLength, unsigned char* response );
};

class SetSessionPrivCMD:public I_Command{
private:
    unsigned char privLvl;
public:
    SetSessionPrivCMD();
    unsigned char getPrivLvl();
    bool setPrivLvl(const unsigned char newLvl);
    int process(const unsigned char* request, int reqLength, unsigned char* response );
};

class ActSessionCMD:public I_Command{
private:
    SetSessionPrivCMD* privCMD;
    unsigned char authType;
public:
    ActSessionCMD(SetSessionPrivCMD* priv);
    bool verifyTempID(const unsigned char* data);
    bool verifyChalString(const unsigned char* data);
    int process(const unsigned char* request, int reqLength, unsigned char* response );
};

class CloseSessionCMD:public I_Command{
public:
    int process(const unsigned char* request, int reqLength, unsigned char* response );
};

class GetSessionInfo:public I_Command {
private:
    SetSessionPrivCMD* privCMD;
    GetChannelAuthCMD* channelCMD;
public:
    GetSessionInfo(SetSessionPrivCMD* priv, GetChannelAuthCMD* channel);
    int process(const unsigned char* request, int reqLength, unsigned char* response );
};

#endif
