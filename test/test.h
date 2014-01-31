#ifndef __TEST_H
#define __TEST_H

#include "cxxtest/TestSuite.h"
#include <iostream>
#include <fstream>

 std::ofstream log_file("J3NI_test_log.log");
//using namespace IpmiCommandDefines;
class J3NITestSuite : public CxxTest::TestSuite 
{
    public:

    void testIpmiMessageValidCreation(void) {
        const unsigned char* msg = IpmiCommandDefines::PONG_RESPONSE;
        IpmiMessage * testMsg = new IpmiMessage(msg, 27);

        TS_ASSERT_EQUALS(testMsg->length(), 27);
        TS_ASSERT_SAME_DATA(testMsg->message(), msg, 27);

    }
    
    void testIsPing(void) {
        IpmiMessage testMsg(IpmiCommandDefines::PING_REQUEST, 12);
        TS_ASSERT(MsgHandler::isPing(testMsg));
    }
    
    
};


#endif
