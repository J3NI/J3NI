#ifndef __TEST_H
#define __TEST_H

#include "cxxtest/TestSuite.h"
#include <iostream>
#include <fstream>

std::ofstream log_file("J3NI_test_log.log");

class J3NITestSuite : public CxxTest::TestSuite 
{
    public:
    void setUp() {
        MsgHandler::initCMD();
    }

    void testIpmiMessageValidCreation(void) {
        const unsigned char* msg = IpmiCommandDefines::PONG_RESPONSE;
        IpmiMessage * testMsg = new IpmiMessage(msg, 27);

        TS_ASSERT_EQUALS(testMsg->length(), 27);
        TS_ASSERT_SAME_DATA(testMsg->message(), msg, 27);

    }
    
    void testIsPing(void) {
        IpmiMessage testMsg(IpmiCommandDefines::PING_REQUEST, 12);
        TS_TRACE("Executing testIsPing");
        TS_ASSERT(MsgHandler::isPing(testMsg));
    }

    void testPong(void) {
        IpmiMessage req(IpmiCommandDefines::PING_REQUEST, 12);
        const unsigned char* msg = IpmiCommandDefines::PONG_RESPONSE;
        IpmiMessage testMsg;
        MsgHandler::pong(req, testMsg);
        TS_ASSERT_SAME_DATA(testMsg.message(), msg, 27);
        TS_ASSERT_EQUALS(testMsg.message()[9], req.message()[9]);
    }
   
    
};


#endif
