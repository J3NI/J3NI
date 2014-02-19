#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include <map>

class I_Command;
class IpmiMessage;



class MsgHandler {
public:
    typedef std::map<unsigned char, I_Command*> CommandMap;
    
private:
    static CommandMap commands_;
    
public:
    static void initCMD();
    static void clearCMD();
    
    static bool isPing(const IpmiMessage& message);
    static void pong(const IpmiMessage& message, IpmiMessage& response);
    static void processRequest(const IpmiMessage& message, IpmiMessage& response);
 };

#endif
