#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include <map>

class IpmiMessage;
class cmdProcessor;

typedef int (cmdProcessor::*cmd)(const unsigned char*, unsigned char*);
typedef std::map<unsigned char, cmd> cmdMap;

class cmdProcessor{
    private:
        cmdMap cmds;
    
    public:
    cmdProcessor();
    int process(const unsigned char& index, const unsigned char* request, unsigned char* response );
    int GetChannelAuthCMD(const unsigned char* request, unsigned char* response);
    int GetSessionChalCMD(const unsigned char* request, unsigned char* response);
    int ActSessionCMD(const unsigned char* request, unsigned char* response);
    int SetSessionPrivCMD(const unsigned char* request, unsigned char* response);
    int CloseSessionCMD(const unsigned char* request, unsigned char* response);

};


class MsgHandler {
    private:

    public:
        static bool isPing(const IpmiMessage& message);
        static void pong(const IpmiMessage& message, IpmiMessage& response);
        static void processRequest(const IpmiMessage& message, IpmiMessage& response);
 };

#endif
