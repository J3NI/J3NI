#ifndef MSGHANDLER_H
#define MSGHANDLER_H

class MsgHandler {
    private:

    public:
        static bool is_ping(unsigned char* );
        static int pong(unsigned char* , unsigned char* );
        static int processRequest(unsigned char* , unsigned char* );
 };

#endif
