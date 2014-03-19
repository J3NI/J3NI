#ifndef __TEST_CHANNEL_H
#define __TEST_CHANNEL_H

#include "cxxtest/TestSuite.h"
#include <iostream>
#include <fstream>


class TestCHANNELSuite : public CxxTest::TestSuite
{
    private:
    unsigned char* blank_request;
    
    public:
    
    static TestCHANNELSuite* createSuite() {
        return new TestCHANNELSuite();
    }
    
    static void destroySuite(TestCHANNELSuite* suite) {
        delete suite;
    }
    
    void setUp(){
        MsgHandler::initCMD();
        blank_request = new unsigned char[IpmiCommandDefines::MAX_DATA_SIZE];
	blank_request[IpmiCommandDefines::NET_FN_INDEX] = 0x18;
    }
    
    void tearDown() {
        MsgHandler::clearCMD();
        delete blank_request;
    }

    //---------- TEST Get Channel Authentication Capabilities   ---------- 
    void testGetChannelAuthCapCMD(void) {
        TS_TRACE("Testing Get Channel Authentication Capabilities CMD");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x38;
	blank_request[IpmiCommandDefines::COMMAND_INDEX+1] = 0x01; //Channel 1 default
	blank_request[IpmiCommandDefines::COMMAND_INDEX+2] = 0x04; // Administrator Level
        IpmiMessage request(blank_request, 23);
	IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
	TS_ASSERT_EQUALS(testResponse.data()[1], 0x01); // Channel number
	TS_ASSERT_EQUALS(testResponse.data()[2], 0x11); // Auth Type
	TS_ASSERT_EQUALS(testResponse.data()[3], 0x1f);
	TS_ASSERT_EQUALS(testResponse.data()[4], 0x00); // reserved for IPMIv1.5
	TS_ASSERT_EQUALS(testResponse.data()[5], 0x00);
	TS_ASSERT_EQUALS(testResponse.data()[6], 0x00);
	TS_ASSERT_EQUALS(testResponse.data()[7], 0x00);
	TS_ASSERT_EQUALS(testResponse.data()[8], 0x00);
    }
    void testGetChannelInfoCMD(void) {
	TS_TRACE("Testing Get Channel Info CMD");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x42;
	blank_request[IpmiCommandDefines::COMMAND_INDEX+1] = 0x01; //Channel 1
        IpmiMessage request(blank_request, 22);
	IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
	TS_ASSERT_EQUALS(testResponse.data()[1], 0x01); // Channel number
	TS_ASSERT_EQUALS(testResponse.data()[2], 0x04); // Channel Medium Type: 802.3 LAN
	TS_ASSERT_EQUALS(testResponse.data()[3], 0x01); // Channel Protocol Type: IPMB-1.0
	TS_ASSERT_EQUALS(testResponse.data()[4], 0x81); // Session support & # sessions activated
	TS_ASSERT_EQUALS(testResponse.data()[5], 0xf2); // IPMI Enterprise Number = 0xF21B00
	TS_ASSERT_EQUALS(testResponse.data()[6], 0x1b);
	TS_ASSERT_EQUALS(testResponse.data()[7], 0x00);
	TS_ASSERT_EQUALS(testResponse.data()[8], 0xFF); // Aux Chan Info: no interrupt/unspecified
	TS_ASSERT_EQUALS(testResponse.data()[9], 0xFF);
    }

    void testGetChannelAccessCMD(void) {
	TS_TRACE("Testing Get Channel Access CMD");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x41;
	blank_request[IpmiCommandDefines::COMMAND_INDEX+1] = 0x01; // Channel 1
	blank_request[IpmiCommandDefines::COMMAND_INDEX+2] = 0x80; // Returns same for volatile and non-volatile
        IpmiMessage request(blank_request, 23);
	IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
	TS_ASSERT_EQUALS(testResponse.data()[1], 0x3f); // 0x3f by default
	TS_ASSERT_EQUALS(testResponse.data()[2], 0x04); // Priv Lvl: ADMIN LVL default
    }

//    void testSetChannelAccessCMD(void) {
//	TS_TRACE("Testing Set Channel Access CMD");
	// Set Channel Acces CMD -> change the settings
//        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x40;
//	blank_request[IpmiCommandDefines::COMMAND_INDEX+1] = 0x3c;
		// 00 don't change chan access
		// 1 leave PEF Alerting disabled
		// 1 leave per-msg auth
		// 1 leave user-lvl auth
		// 010 change access mode to always available	
//	blank_request[IpmiCommandDefines::COMMAND_INDEX+2] = 0x42;
		// 0100 Set non-volatile priv lvl limit
		// 0x2 set to User lvl
//        IpmiMessage request(blank_request, 23);
//	IpmiMessage testResponse;
//        MsgHandler::processRequest(request, testResponse);
//        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
	// Get Channel Access CMD -> should display changes
//        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x41;
//	blank_request[IpmiCommandDefines::COMMAND_INDEX+1] = 0x01; // Channel 1
//	blank_request[IpmiCommandDefines::COMMAND_INDEX+2] = 0x40; // Get non-volatile access
//        IpmiMessage request1(blank_request, 23);
//	IpmiMessage testResponse1;
//        MsgHandler::processRequest(request1, testResponse1);
//        TS_ASSERT_EQUALS(testResponse1.data()[0], IpmiCommandDefines::COMP_CODE_OK);
//	TS_ASSERT_EQUALS(testResponse1.data()[1], 0x3c);
//	TS_ASSERT_EQUALS(testResponse1.data()[2], 0x02); // Priv Lvl: USER LVL
//    }

};

#endif
