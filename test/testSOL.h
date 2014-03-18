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
        return new TestSOLSuite();
    }
    
    static void destroySuite(TestSOLSuite* suite) {
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
   

    //---------------------------------------------------
    //---------- TEST Get SOL Configuration    ---------- 
    void testGetSoLConfig(void) {
        TS_TRACE("Testing Get SoL Configuration CMD -- default return expected");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x22;
	//blank_request[IpmiCommandDefines::COMMAND_INDEX+1] = 0x01; //Channel 1 default
	TS_WARN("What the heck");
        IpmiMessage request(blank_request, 25);
	IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK); //Can't even get this to not fail? 
    }

};

#endif
