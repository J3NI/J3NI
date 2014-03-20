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
        //MsgHandler::initCMD();
        return new TestChassisSuite();
    }
    
    static void destroySuite(TestChassisSuite* suite) {
        //MsgHandler::clearCMD();
        delete suite;
    }
    
    void setUp(){
        MsgHandler::initCMD();
        blank_request = new unsigned char[IpmiCommandDefines::MAX_DATA_SIZE];
        blank_request[IpmiCommandDefines::NET_FN_INDEX] = 0x00;
    }
    
    void tearDown() {
        MsgHandler::clearCMD();
        delete blank_request;
    }
    
    //---------------------------------------------------
    //---------------- HELPER FUNCTIONS -----------------
    
    unsigned char getReference(unsigned char cmd, int byte){
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = cmd;
        IpmiMessage request0(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request0, testResponse);
        return testResponse.data()[byte];
    }
    
    void verifyChassisStatORCapabCMD(unsigned char cmd, unsigned char* expectedReturn, int lengthToCheck){
        IpmiMessage testResponse;
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = cmd;
        IpmiMessage request(blank_request, 21);
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        for (int i = 1; i <= lengthToCheck; i++){
            TS_ASSERT_EQUALS(testResponse.data()[i], expectedReturn[i-1]);
        }
    }
    
    void verifyResetCauseCMD(unsigned char expectedReturn){
        IpmiMessage testResponse;
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x07;
        IpmiMessage request(blank_request, 21);
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.data()[1], expectedReturn);
    }
    
    void verifyHoursCMD(unsigned char* expectedReturn, int lengthToCheck){
        IpmiMessage testResponse;
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x0F;
        IpmiMessage request(blank_request, 21);
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_SAME_DATA(testResponse.data(), expectedReturn, lengthToCheck);
    }
    
    void verifyIdentifyCMD(unsigned char* reqData, int reqLength, unsigned char mask,  unsigned char exp){
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x04;
        for (int i = 0; i < reqLength; i++) blank_request[IpmiCommandDefines::DATA_START_INDEX+i] = reqData[i];
        IpmiMessage request(blank_request, 21+reqLength);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);
        if ((testResponse.data()[3] & 0x40 )== 0x40){
            TS_TRACE("\tIdentify status supported; Checking misc. state - identify off");
            TS_ASSERT_EQUALS(testResponse.data()[3] & mask, exp);
        }
    }
    
    void verifyValidReset(void){
        IpmiMessage testResponse;
        
        TS_TRACE("\tChecking power on status, last event, etc.");
        unsigned char expectedReturn[3] = {0x61, 0x00, 0x00};
        verifyChassisStatORCapabCMD(0x01, expectedReturn, 3);
        
        TS_TRACE("\tChecking reset cause");
        verifyResetCauseCMD(0x01);
        
        TS_TRACE("\tChecking successful POH reset");
        unsigned char expectedPOH[6] = {0x00, 0x3c, 0x00, 0x00, 0x00, 0x00};
        verifyHoursCMD(expectedPOH, 6);
    }
    
    void verifyFrontPanelCMD(unsigned char reqData, unsigned char referenceFrontPanel){
        unsigned char mask = reqData<<4;
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x0A;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = reqData;
        IpmiMessage request(blank_request, 22);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request2(blank_request, 21);
        MsgHandler::processRequest(request2, testResponse);
        if (testResponse.length() == 26) {
            TS_TRACE("\tStatus CMD supports Front panel info. Checking byte 5");
            if ((referenceFrontPanel&mask) == mask) {
                if (reqData == 0x00) {
                    TS_ASSERT_EQUALS(testResponse.data()[4]&referenceFrontPanel, referenceFrontPanel&0xF0);
                } else TS_ASSERT_EQUALS(testResponse.data()[4]&referenceFrontPanel, referenceFrontPanel|reqData);
            } else
                TS_ASSERT_EQUALS(testResponse.data()[4], referenceFrontPanel);
        } else
            TS_WARN("Status CMD doesn't support Front Panel Status");
    }
    
    void verifySetChassisCMD(unsigned char* reqData, int reqLength){
        unsigned char referenceByte = getReference(0x00, 1);
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x05;
        for (int i = 0; i < reqLength; i++) blank_request[IpmiCommandDefines::DATA_START_INDEX+i] = reqData[i];
        IpmiMessage request(blank_request, 21+reqLength);
        IpmiMessage testResponse;
        if (reqLength < 5 || reqLength > 6) {
            reqLength = 5;
            for (int i = 1; i <=5; i++)
                reqData[i] = getReference(0x00, i+1);
        }
        else if (reqLength > 6) {
            reqLength = 6;
            reqData[5] = getReference(0x00, 6);
        }
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        reqData[0] = referenceByte;
        TS_WARN("Chassis Capabilities Flag is a read only value in this implementation");
        verifyChassisStatORCapabCMD(0x00, reqData, reqLength);
    }
    
    void verifyPowerRestoreCMD(unsigned char dataByte, unsigned char expByte){
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x06;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = dataByte;
        IpmiMessage request(blank_request, 22);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.data()[1], dataByte);
        TS_ASSERT_EQUALS(getReference(0x01,1), expByte);
    }
    
    void poweroff(){
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x00;
        IpmiMessage request(blank_request, 22);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
    }
    //---------------------------------------------------
    //---------- TEST Get Chassis Capabilities ---------- 
    
    void testGetChassisCapab_NoBridge(void) {
        TS_TRACE("Testing Chassis Capabilities CMD -- default (no bridge) return expected");
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x00;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 27);
    }

    //---------------------------------------------------
    //------------- TEST Get Chassis Status -------------
    
    void testGetChassisStatus(void) {
        TS_TRACE("Testing Get Chassis Status CMD -- default return expected");
        unsigned char expectedReturn[3] = {0x61, 0x00, 0x00};
        verifyChassisStatORCapabCMD(0x01, expectedReturn, 3);
    }
    
    //---------------------------------------------------
    //--------------- TEST Chassis Control --------------
    void testChassisControl_PowerOff(void) {
        TS_TRACE("Testing Chassis Control CMD with power off request -- 'not allowed in current state' error expected");
        
        poweroff();
        
        TS_TRACE("\tChecking power off status");
        unsigned char expectedReturn[1] = {0x60};
        verifyChassisStatORCapabCMD(0x01, expectedReturn, 1);
    }
    
    void testChassisControl_InvalidCycle(void) {
        TS_TRACE("Testing Chassis Control CMD with cycle request in powered off state -- 'not allowed in current state' error expected");
        poweroff();
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x02;
        IpmiMessage request(blank_request, 22);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::CANNOT_EXEC_IN_CUR_STATE);
    }

    void testChassisControl_InvalidReset(void) {
        TS_TRACE("Testing Chassis Control CMD with reset request in powered off state");
        poweroff();
        
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
        unsigned char expectedReturn[3] = {0x61, 0x10, 0x00};
        verifyChassisStatORCapabCMD(0x01, expectedReturn, 3);
    }
    
    void testChassisControl_Cycle(void) {
        TS_TRACE("Testing Chassis Control CMD with power cycle request in powered on state");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x02;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        
        verifyValidReset();        
    }
    
    void testChassisControl_Reset(void) {
        TS_TRACE("Testing Chassis Control CMD with reset request in powered on state");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x03;
        IpmiMessage request(blank_request, 22);
        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        
        verifyValidReset();
    }

    //---------------------------------------------------
    //--------------- TEST Chassis Reset ----------------
    
   void testChassisReset(void){
        TS_TRACE("Testing Chassis Reset CMD --'disabled' error code expected");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x03;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);

        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::CANNOT_EXEC_DISABLED);
        TS_ASSERT_EQUALS(testResponse.length(), 22);
   }

    //---------------------------------------------------
    //------------- TEST Chassis Identify ---------------
    
   void testChassisIdentify_NoArgs(void){
       TS_TRACE("Testing Chassis Identify CMD with no arguments");
       verifyIdentifyCMD(NULL, 0, 0x00,  0x00); // Change mask & exp. value if implementing timed identify
       TS_WARN("No timed identify -- TO DO: check 15 sec identify interval");
   }
    
    void testChassisIdentify_IntervalArg(void){
        TS_TRACE("Testing Chassis Identify CMD with (optional) interval request byte");
        unsigned char reqData[1] = {0x01};
        verifyIdentifyCMD(reqData, 1, 0x00,  0x00); // Change mask & exp. value if implementing timed identify
        TS_WARN("No timed identify -- TO DO: check 15 sec identify interval");
    }
    
    void testChassisIdentify_Off(void){
        TS_TRACE("Testing Chassis Identify CMD with off request");
        unsigned char reqData[1] = {0x00};
        verifyIdentifyCMD(reqData, 1, 0x70,  0x40);
    }
    
    void testChassisIdentify_ForceArg(void){
        TS_TRACE("Testing Chassis Identify CMD with (optional) force identify request byte");
        TS_WARN("Actual BMC may return an error if the 'force' option is not supported");
        unsigned char reqData[2] = {0x01, 0x01};
        verifyIdentifyCMD(reqData, 2, 0x70,  0x60); // Change mask & exp. value if implementing timed identify
    }
    
    //---------------------------------------------------
    //-------- TEST Chassis Front Panel Enables ---------
    
    void testChassisFrontPanel_disableStandby(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable standby request");
        verifyFrontPanelCMD(0x08, getReference(0x01, 4));
    }
    
    void testChassisFrontPanel_disableDiagnostic(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable diagnostic request");
        verifyFrontPanelCMD(0x04, getReference(0x01, 4));
    }
    void testChassisFrontPanel_disableDiagnosticAndStandby(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable diagnostic & standby request");
        verifyFrontPanelCMD(0x0C, getReference(0x01, 4));
    }
    void testChassisFrontPanel_disableReset(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable reset request");
        verifyFrontPanelCMD(0x02, getReference(0x01, 4));
    }
    void testChassisFrontPanel_disableDiagnosticAndReset(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable diagnostic & reset request");
        verifyFrontPanelCMD(0x06, getReference(0x01, 4));
    }
    void testChassisFrontPanel_disableResetAndStandby(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable reset & standby request");
        verifyFrontPanelCMD(0x0A, getReference(0x01, 4));
    }
    void testChassisFrontPanel_disableDiagnosticResetStandby(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable diagnostic, reset & standby request");
        verifyFrontPanelCMD(0x0D, getReference(0x01, 4));
    }
    void testChassisFrontPanel_disablePowerOff(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable power off request");
        verifyFrontPanelCMD(0x01, getReference(0x01, 4));
    }
    void testChassisFrontPanel_disableStandbyAndPowerOff(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable standby & power off request");
        verifyFrontPanelCMD(0x09, getReference(0x01, 4));
    }
    void testChassisFrontPanel_disableDiagnosticAndPowerOff(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable diagnostic & power off request");
        verifyFrontPanelCMD(0x05, getReference(0x01, 4));
    }
    void testChassisFrontPanel_disableResetAndPowerOff(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable reset & power off request");
        verifyFrontPanelCMD(0x05, getReference(0x01, 4));
    }
    void testChassisFrontPanel_disableAll(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable all buttons request");
        verifyFrontPanelCMD(0x0F, getReference(0x01, 4));
    }
    void testChassisFrontPanel_enableAll(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with enable all buttons request");
        verifyFrontPanelCMD(0x00, getReference(0x01, 4));
    }
    
    //---------------------------------------------------
    //---------- TEST Set Chassis Capabilities ----------
    void testSetChassisCapabilities_NoBridge(void){
        unsigned char reqData[5] = {0x03, 0xFA, 0xEB, 0xDC, 0x23 };
        verifySetChassisCMD(reqData, 5);
    }
    
    void testSetChassisCapabilities_Full(void){
        unsigned char reqData[6] = {0x03, 0xFA, 0xEB, 0xDC, 0x23, 0x78 };
        verifySetChassisCMD(reqData, 6);
    }
    
    void testSetChassisCapabilities_TooShort(void){
        unsigned char reqData[3] = {0x03, 0xFA, 0xEB };
        verifySetChassisCMD(reqData, 3);
    }
    
    void testSetChassisCapabilities_TooLong(void){
        unsigned char reqData[8] = {0x03, 0xFA, 0xEB, 0xDC, 0x23, 0x78, 0x20, 0x04 };
        verifySetChassisCMD(reqData, 8);
    }
    //---------------------------------------------------
    //------ TEST Set Chassis Power Restore Policy ------
    void testChassisPowerRestore_NoChange(void){
        unsigned char refPower = getReference(0x01, 1);
        verifyPowerRestoreCMD(0x03,refPower);
    }

    void testChassisPowerRestore_AlwaysUp(void){
        unsigned char refPower = getReference(0x01, 1);
        refPower = (refPower&0x9F)|0x40;
        verifyPowerRestoreCMD(0x02,refPower);
    }
    void testChassisPowerRestore_PrevState(void){
        unsigned char refPower = getReference(0x01, 1);
        refPower = (refPower&0x9F)|0x20;
        verifyPowerRestoreCMD(0x01,refPower);
    }
    void testChassisPowerRestore_Off(void){
        unsigned char refPower = getReference(0x01, 1);
        refPower = refPower&0x9F;
        verifyPowerRestoreCMD(0x00,refPower);
    }
    //---------------------------------------------------
    //---------- TEST Set Chassis Power Cycle -----------
    void testChassisPowerCycle(void){
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x0B;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x00;
        IpmiMessage request(blank_request, 22);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
    }
    
    //---------------------------------------------------
    //------ TEST Get Chassis System Restart Cause ------
    void testChassisRestartCause(void){
        unsigned char prevCause = getReference(0x0B, 1);
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x07;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.data()[1], prevCause);
        TS_ASSERT_EQUALS(testResponse.length(), 24);
    }
    
    //---------------------------------------------------
    //---------- TEST Set Chassis POH Counter -----------
    void testChassisPOH(void){
        unsigned char prevCause = getReference(0x0B, 1);
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x0F;
        IpmiMessage request(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.data()[1], 60);
        TS_ASSERT_EQUALS(testResponse.length(), 27);
    }
    
};


#endif
