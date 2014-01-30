#ifndef MSGHANDLER_H
#define MSGHANDLER_H

class IpmiMessage;

class MsgHandler {
    private:

    public:
        static bool isPing(const IpmiMessage& message);
        static void pong(const IpmiMessage& message, IpmiMessage& response);
        static void processRequest(const IpmiMessage& message, IpmiMessage& response);
 };

#endif
