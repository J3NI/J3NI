#ifndef __TEST_LAN_H
#define __TEST_LAN_H

#include "cxxtest/TestSuite.h"
#include <iostream>
#include <fstream>


class TestLANSuite : public CxxTest::TestSuite
{
    private:
    unsigned char* blank_request;
    
    public:
    
    static TestLANSuite* createSuite() {
        return new TestLANSuite();
    }
    
    static void destroySuite(TestLANSuite* suite) {
        delete suite;
    }
    
    void setUp(){
        MsgHandler::initCMD();
        blank_request = new unsigned char[IpmiCommandDefines::MAX_DATA_SIZE];
	blank_request[IpmiCommandDefines::NET_FN_INDEX] = 0x30;
    }
    
    void tearDown() {
        MsgHandler::clearCMD();
        delete blank_request;
    }
   

    //---------------------------------------------------
    //---------- TEST Get LAN Configuration    ---------- 
    void testGetLANConfig(void) {
        TS_TRACE("Testing Get LAN Configuration CMD -- default return expected");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
	blank_request[IpmiCommandDefines::COMMAND_INDEX+1] = 0x01; //Channel 1 default
	blank_request[IpmiCommandDefines::COMMAND_INDEX+2] = 0x03; //
        IpmiMessage request(blank_request, 25);
	IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
	TS_ASSERT_EQUALS(testResponse.data()[1], 0x00);
	TS_ASSERT_EQUALS(testResponse.data()[2], 0x7F);
	TS_ASSERT_EQUALS(testResponse.data()[3], 0x00);
	TS_ASSERT_EQUALS(testResponse.data()[4], 0x00);
	TS_ASSERT_EQUALS(testResponse.data()[5], 0x01);
    }

    //---------------------------------------------------
    //---------- TEST Set LAN Configuration    ---------- 
    void testSetLANConfig(void){
	TS_TRACE("Testing Set LAN Configuration CMD");
	blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
	blank_request[IpmiCommandDefines::COMMAND_INDEX+1] = 0x01; //Channel 1 default
	blank_request[IpmiCommandDefines::COMMAND_INDEX+2] = 0x00; //Change Set-in-progress
	blank_request[IpmiCommandDefines::COMMAND_INDEX+3] = 0x01; //change to set in progress
	IpmiMessage request(blank_request, 25);
	IpmiMessage testResponse;
	MsgHandler::processRequest(request, testResponse);
	TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK); 
	
    }

    
};

#endif
