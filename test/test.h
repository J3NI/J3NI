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
	TS_TRACE("Executing testIpmiMessageValidCreation");
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
	TS_TRACE("Executing testPong");
        IpmiMessage req(IpmiCommandDefines::PING_REQUEST, 12);
        const unsigned char* msg = IpmiCommandDefines::PONG_RESPONSE;
        IpmiMessage testMsg;
        MsgHandler::pong(req, testMsg);
        TS_ASSERT_SAME_DATA(testMsg.message(), msg, 27);
        TS_ASSERT_EQUALS(testMsg.message()[9], req.message()[9]);
    }

   void testChassis(void){
	TS_TRACE("Executing testChassis");
	IpmiMessage req(IpmiCommandDefines::GET_CHASSIS_STATUS,21);
	int powerOn = 0x61;
	int lastpowerEvent = 0x00;
	int chassisState = 0x00;
	IpmiMessage testChassisStatus;
	MsgHandler::processRequest(req, testChassisStatus);
	TS_ASSERT(req.message()[19] == 0x01);
    }
   
    
};


#endif
