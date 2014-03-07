#ifndef __TEST_CHASSIS_H
#define __TEST_CHASSIS_H

#include "cxxtest/TestSuite.h"
#include <iostream>
#include <fstream>

class TestChassisSuite : public CxxTest::TestSuite
{
    private:
    unsigned char* blank_request;
    unsigned char* response;
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
        response = new unsigned char[IpmiCommandDefines::MAX_DATA_SIZE];
    }
    
    void tearDown() {
        delete blank_request;
        delete response;
    }
    
    void testGetChassisCapab_NoBridge(void) {
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x00;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 27);
    }

    void testGetChassisStatus(void) {
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        unsigned char powerState = 0x61;
        unsigned char lastPower = 0x00;
        unsigned char miscChassis = 0x00;
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 25);

        TS_ASSERT_EQUALS(testResponse.data()[1], powerState);
        TS_ASSERT_EQUALS(testResponse.data()[2], lastPower);
        TS_ASSERT_EQUALS(testResponse.data()[3], miscChassis);

    }
    
    void testChassisControl_PowerOff(void) {
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x00;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);

        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x60);
    }
    
    void testChassisControl_InvalidCycle(void) {
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x02;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::CANNOT_EXEC_IN_CUR_STATE);
    }

    void testChassisControl_InvalidReset(void) {
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x03;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::CANNOT_EXEC_IN_CUR_STATE);
    }
    
    void testChassisControl_PowerOn(void) {
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x01;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);

        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 25);
        
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x61);
        TS_ASSERT_EQUALS(testResponse.data()[2], 0x10);
        TS_ASSERT_EQUALS(testResponse.data()[3], 0x00);
    }
    
    void testChassisControl_Cycle(void) {
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x02;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 25);
        
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x61);
        TS_ASSERT_EQUALS(testResponse.data()[2], 0x00);
        TS_ASSERT_EQUALS(testResponse.data()[3], 0x00);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x07;
        IpmiMessage request3(blank_request, 21);
        MsgHandler::processRequest(request3, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x01);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x0F;
        IpmiMessage request4(blank_request, 21);
        MsgHandler::processRequest(request4, testResponse);
        unsigned char expectedPOH[6] = {0x00, 0x3c, 0x00, 0x00, 0x00, 0x00};
        TS_ASSERT_SAME_DATA(testResponse.data(), expectedPOH, 6);
    }
    
    void testChassisControl_Reset(void) {
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x03;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 25);
        
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x61);
        TS_ASSERT_EQUALS(testResponse.data()[2], 0x00);
        TS_ASSERT_EQUALS(testResponse.data()[3], 0x00);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x07;
        IpmiMessage request3(blank_request, 21);
        MsgHandler::processRequest(request3, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.data()[1], 0x01);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x0F;
        IpmiMessage request4(blank_request, 21);
        MsgHandler::processRequest(request4, testResponse);
        unsigned char expectedPOH[6] = {0x00, 0x3c, 0x00, 0x00, 0x00, 0x00};
        TS_ASSERT_SAME_DATA(testResponse.data(), expectedPOH, 6);
    }
    
};


#endif
