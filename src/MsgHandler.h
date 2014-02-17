#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include <map>

class IpmiMessage;
class cmdProcessor;

typedef std::map<unsigned char, cmdProcessor*> cmdMap;

class cmdProcessor{
    public:
    virtual int process(const unsigned char* request, unsigned char* response ) = 0;
};

class MsgHandler {
    private:

    public:
        static void initCMD();
        static bool isPing(const IpmiMessage& message);
        static void pong(const IpmiMessage& message, IpmiMessage& response);
        static void processRequest(const IpmiMessage& message, IpmiMessage& response);
 };

#endif
