#ifndef __TEST_SOL_H
#define __TEST_SOL_H

#include "cxxtest/TestSuite.h"
#include <iostream>
#include <fstream>


class TestSOLSuite : public CxxTest::TestSuite
{
    private:
    unsigned char* blank_request;
    
    public:
    
    static TestSOLSuite* createSuite() {
        MsgHandler::initCMD();
        return new TestSOLSuite();
    }
    
    static void destroySuite(TestSOLSuite* suite) {
        MsgHandler::clearCMD();
        delete suite;
    }
    
    void setUp(){
        blank_request = new unsigned char[IpmiCommandDefines::MAX_DATA_SIZE];
        // Set Net Function byte to 00110000
        // Will be processed as NetFn >> 2
        // So will result in 00001100 (which is 0x0C) corresponding to the transport NetFn category
        blank_request[IpmiCommandDefines::NET_FN_INDEX] = 0x30;
    }
    
    void tearDown() {
        delete blank_request;
    }
   

    //---------------------------------------------------
    //---------- TEST Get SOL Configuration    ---------- 
    void testGetSoLConfig(void) {
        TS_TRACE("Testing Get SoL Configuration CMD -- default return expected");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x22;
	blank_request[IpmiCommandDefines::COMMAND_INDEX+1] = 0x01; //Channel 1 default
	blank_request[IpmiCommandDefines::COMMAND_INDEX+2] = 0x05; //Check NV bit rate
        IpmiMessage request(blank_request, 25);
	IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK); 
	TS_ASSERT_EQUALS(testResponse.data()[1], 0x00) // Parameter Revision should be 0
	TS_ASSERT_EQUALS(testResponse.data()[2], 0x07) // default bit rate value
    }

    //---------------------------------------------------
    //---------- TEST Set SOL Configuration    ---------- 
    void testSetSolConfig(void){
	TS_TRACE("Testing Set SoL Configuration CMD");
	blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x21;
	blank_request[IpmiCommandDefines::COMMAND_INDEX+1] = 0x01; //Channel 1 default
	blank_request[IpmiCommandDefines::COMMAND_INDEX+2] = 0x05; //Change NV bit rate
	blank_request[IpmiCommandDefines::COMMAND_INDEX+3] = 0x06; //change it to 6
	IpmiMessage request(blank_request, 25);
	IpmiMessage testResponse;
	MsgHandler::processRequest(request, testResponse);
	TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK); 
	
	TS_TRACE("Check functionality");
	//Test if it actually set stuff, maybe a better way to check other than use the get func?
	blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x22;
	blank_request[IpmiCommandDefines::COMMAND_INDEX+1] = 0x01; //Channel 1 default
	blank_request[IpmiCommandDefines::COMMAND_INDEX+2] = 0x05; //Check NV bit rate
        IpmiMessage request2(blank_request, 25);
	IpmiMessage testResponse2;
        MsgHandler::processRequest(request2, testResponse2);
        TS_ASSERT_EQUALS(testResponse2.data()[0], IpmiCommandDefines::COMP_CODE_OK); 
	TS_ASSERT_EQUALS(testResponse2.data()[1], 0x01) // Parameter Revision should be 1
	TS_ASSERT_EQUALS(testResponse2.data()[2], 0x06) // default bit rate value
    }
};

#endif
