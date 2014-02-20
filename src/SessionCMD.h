#ifndef SESSIONCMD_H
#define SESSIONCMD_H

#include <I_Command.h>

class GetSessionChalCMD:public I_Command{
public:
    int process(const unsigned char* request, int reqLength, unsigned char* response );
};
class ActSessionCMD:public I_Command{
public:
    int process(const unsigned char* request, int reqLength, unsigned char* response );
};
class SetSessionPrivCMD:public I_Command{
public:
    int process(const unsigned char* request, int reqLength, unsigned char* response );
};
class CloseSessionCMD:public I_Command{
public:
    int process(const unsigned char* request, int reqLength, unsigned char* response );
};

#endif