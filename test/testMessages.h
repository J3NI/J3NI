#ifndef __TEST_MESSAGES_H
#define __TEST_MESSAGES_H

#include "cxxtest/TestSuite.h"
#include <iostream>
#include <fstream>

std::ofstream log_file("J3NI_test_log.log");

class TestMessages : public CxxTest::TestSuite
{
    private:
    unsigned char* blank_request;
    
    public:
    
    static TestMessages* createSuite() {
        return new TestMessages();
    }
    
    static void destroySuite(TestMessages* suite) {
        delete suite;
    }
    
    void setUp(){
        MsgHandler::initCMD();
        blank_request = new unsigned char[IpmiCommandDefines::MAX_DATA_SIZE];
    }
    
    void tearDown() {
        MsgHandler::clearCMD();
        delete blank_request;
    }
    //------------------------------------------------------
    void testIpmiMessageValidCreation(void) {
	TS_TRACE("Executing testIpmiMessageValidCreation");
        const unsigned char* msg = IpmiCommandDefines::PONG_RESPONSE;
        IpmiMessage * testMsg = new IpmiMessage(msg, 27);

        TS_ASSERT_EQUALS(testMsg->length(), 27);
        TS_ASSERT_SAME_DATA(testMsg->message(), msg, 27);

    }
    
    void testIpmiDataValid(void){
	TS_TRACE("Checking Data Functions");
	unsigned char msg[25] = {};
	msg[IpmiCommandDefines::COMMAND_INDEX] = 5;
	msg[20] = 1;
	msg[21] = 2;
	msg[22] = 3;
	msg[23] = 4;
	msg[24] = 5;
	IpmiMessage * testMsg = new IpmiMessage(msg,25);
	TS_ASSERT_EQUALS(testMsg->dataLength(), 4);
	TS_ASSERT_EQUALS(testMsg->data()[1], 2);
	
    }

    void testIpmiGetNetFn(void){
	TS_TRACE("Checking NetFn getting");
	unsigned char msg[25] = {};
	msg[IpmiCommandDefines::NET_FN_INDEX] = 0x0c;
	IpmiMessage * testMsg = new IpmiMessage(msg,25);
	TS_ASSERT_EQUALS(testMsg->getNetFn(), 0x0c >> 2); //Whys it shifting 2?
    }

    void testIpmiOperator(void){
	TS_TRACE("Checking operator funcion");
	unsigned char msg[25] = {};
	msg[6] = 10;
	IpmiMessage * testMsg = new IpmiMessage(msg,25);
	TS_ASSERT_EQUALS(testMsg->operator[](6), 10);
    }

    void testIpmiSetMsg(void){
	TS_TRACE("Checking set Msg funcion");
	unsigned char msg[25] = {};
	IpmiMessage * testMsg = new IpmiMessage();
	TS_ASSERT_EQUALS(testMsg->setMessage(msg, 25), true);
	unsigned char msg2[20] = {};
	IpmiMessage * testMsg2 = new IpmiMessage();
	TS_ASSERT_EQUALS(testMsg->setMessage(msg, 20), false);
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
    
};


#endif
