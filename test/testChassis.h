#ifndef __TEST_CHASSIS_H
#define __TEST_CHASSIS_H

#include "cxxtest/TestSuite.h"
#include <iostream>
#include <fstream>

class TestChassisSuite : public CxxTest::TestSuite
{
    private:
    unsigned char* blank_request;
    public:
    
    static TestChassisSuite* createSuite() {
        MsgHandler::initCMD();
        return new TestChassisSuite();
    }
    
    static void destroySuite(TestChassisSuite* suite) {
        MsgHandler::clearCMD();
        delete suite;
    }
    
    void setUp(){
        blank_request = new unsigned char[IpmiCommandDefines::MAX_DATA_SIZE];
    }
    
    void tearDown() {
        delete blank_request;
    }
    
    void testGetChassisCapab_NoBridge(void) {
        TS_TRACE("Testing Chassis Capabilities CMD -- default (no bridge) return expected");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x00;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 27);
    }

    void testGetChassisStatus(void) {
        TS_TRACE("Testing Get Chassis Status CMD -- default return expected");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        unsigned char powerState = 0x61;
        unsigned char lastPower = 0x00;
        unsigned char miscChassisMask = 0xBF;
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 25);

        TS_ASSERT_EQUALS(testResponse.data()[1], powerState);
        TS_ASSERT_EQUALS(testResponse.data()[2], lastPower);
        TS_ASSERT_EQUALS(testResponse.data()[3] & miscChassisMask, 0x00);

    }
    
    void testChassisControl_PowerOff(void) {
        TS_TRACE("Testing Chassis Control CMD with power off request -- 'not allowed in current state' error expected");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x00;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        TS_TRACE("\tChecking power off status");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x60);
    }
    
    void testChassisControl_InvalidCycle(void) {
        TS_TRACE("Testing Chassis Control CMD with cycle request in powered off state -- 'not allowed in current state' error expected");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x02;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::CANNOT_EXEC_IN_CUR_STATE);
    }

    void testChassisControl_InvalidReset(void) {
        TS_TRACE("Testing Chassis Control CMD with reset request in powered off state");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x03;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::CANNOT_EXEC_IN_CUR_STATE);
    }
    
    void testChassisControl_PowerOn(void) {
        TS_TRACE("Testing Chassis Control CMD with power on request");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x01;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        
        TS_TRACE("\tChecking power on status, last event, etc.");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 25);
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x61);
        TS_ASSERT_EQUALS(testResponse.data()[2], 0x10);
        TS_ASSERT_EQUALS(testResponse.data()[3] & 0xBF, 0x00);
    }
    
    void testChassisControl_Cycle(void) {
        TS_TRACE("Testing Chassis Control CMD with power cycle request in powered on state");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x02;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        
        TS_TRACE("\tChecking power on status, last event, etc.");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 25);
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x61);
        TS_ASSERT_EQUALS(testResponse.data()[2], 0x00);
        TS_ASSERT_EQUALS(testResponse.data()[3] & 0xBF, 0x00);
        
        TS_TRACE("\tChecking reset cause");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x07;
        IpmiMessage request3(blank_request, 21);
        MsgHandler::processRequest(request3, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x01);
        
        TS_TRACE("\tChecking successful POH reset");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x0F;
        IpmiMessage request4(blank_request, 21);
        MsgHandler::processRequest(request4, testResponse);
        unsigned char expectedPOH[6] = {0x00, 0x3c, 0x00, 0x00, 0x00, 0x00};
        TS_ASSERT_SAME_DATA(testResponse.data(), expectedPOH, 6);
    }
    
    void testChassisControl_Reset(void) {
        TS_TRACE("Testing Chassis Control CMD with reset request in powered on state");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x03;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        
        TS_TRACE("\tChecking power on status, last event, etc.");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 25);        
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x61);
        TS_ASSERT_EQUALS(testResponse.data()[2], 0x00);
        TS_ASSERT_EQUALS(testResponse.data()[3] & 0xBF, 0x00);
        
        TS_TRACE("\tChecking reset cause");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x07;
        IpmiMessage request3(blank_request, 21);
        MsgHandler::processRequest(request3, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x01);
        
        TS_TRACE("\tChecking successful POH reset");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x0F;
        IpmiMessage request4(blank_request, 21);
        MsgHandler::processRequest(request4, testResponse);
        unsigned char expectedPOH[6] = {0x00, 0x3c, 0x00, 0x00, 0x00, 0x00};
        TS_ASSERT_SAME_DATA(testResponse.data(), expectedPOH, 6);
    }
   
   void testChassisReset(void){
        TS_TRACE("Testing Chassis Reset CMD --'disabled' error code expected");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x03;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);

        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::CANNOT_EXEC_DISABLED);
        TS_ASSERT_EQUALS(testResponse.length(), 22);
   }

   void testChassisIdentify_NoArgs(void){
       TS_TRACE("Testing Chassis Identify CMD with no arguments");
       blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x04;
       IpmiMessage request(blank_request, 21);
       IpmiMessage testResponse;
       MsgHandler::processRequest(request, testResponse);

       TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
       
       blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
       IpmiMessage request2(blank_request, 21);
       MsgHandler::processRequest(request2, testResponse);
       if ((testResponse.data()[3] & 0x40) == 0x40){
           TS_TRACE("\tIdentify status supported; Checking misc. state - timed identify");
           TS_FAIL("No timed identify -- TO DO: check 15 sec identify interval");
       }
   }
    
    void testChassisIdentify_IntervalArg(void){
        TS_TRACE("Testing Chassis Identify CMD with (optional) interval request byte");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x04;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x01;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);
        if ((testResponse.data()[3] & 0x40 )== 0x40){
            TS_TRACE("\tIdentify status supported; Checking misc. state - timed identify");
            TS_FAIL("No timed identify -- TO DO: check specified identify interval");
        }
        
    }
    
    void testChassisIdentify_Off(void){
        TS_TRACE("Testing Chassis Identify CMD with off request");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x04;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x00;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);
        if ((testResponse.data()[3] & 0x40 )== 0x40){
            TS_TRACE("\tIdentify status supported; Checking misc. state - identify off");
            TS_ASSERT_EQUALS(testResponse.data()[3] & 0x70, 0x40);
        }
    }
    
    void testChassisIdentify_ForceArg(void){
        TS_TRACE("Testing Chassis Identify CMD with (optional) force identify request byte");
        TS_WARN("Actual BMC may return an error if the 'force' option is not supported");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x04;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x01;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x01;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);
        if ((testResponse.data()[3] & 0x40) == 0x40){
            TS_TRACE("\tIdentify status supported; Checking misc. state - indefinite identify");
            TS_ASSERT_EQUALS(testResponse.data()[3] & 0x70, 0x60);
        }
    }
};


#endif
