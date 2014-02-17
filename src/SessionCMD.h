#ifndef SESSIONCMD_H
#define SESSIONCMD_H

class GetSessionChalCMD:public cmdProcessor{
public:
    int process(const unsigned char* request, unsigned char* response );
};
class ActSessionCMD:public cmdProcessor{
public:
    int process(const unsigned char* request, unsigned char* response );
};
class SetSessionPrivCMD:public cmdProcessor{
public:
    int process(const unsigned char* request, unsigned char* response );
};
class CloseSessionCMD:public cmdProcessor{
public:
    int process(const unsigned char* request, unsigned char* response );
};

#endif