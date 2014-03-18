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
    void testGetSoLConfig_SetInProgress(void) {
        TS_TRACE("Testing Get SoL Configuration CMD -- default return expected");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x22;
	    blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x01; //Channel 1 default
	    blank_request[IpmiCommandDefines::DATA_START_INDEX+1] = 0x01; //Set in progress
	TS_WARN("What the heck");
        IpmiMessage request(blank_request, 25);
	IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK); //Can't even get this to not fail? 
    }

};

#endif
