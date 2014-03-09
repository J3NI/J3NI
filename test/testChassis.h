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
    
    //---------------------------------------------------
    //---------------- HELPER FUNCTIONS -----------------
    
    unsigned char getReferenceFrontPanel(){
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request0(blank_request, 21);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request0, testResponse);
        return testResponse.data()[4];
    }
    
    void verifySessionCMD(unsigned char* expectedReturn, int lengthToCheck){
        TS_TRACE("\tChecking power on status, last event, etc.");
        
        IpmiMessage testResponse;
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x01;
        IpmiMessage request(blank_request, 21);
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse.data()[0], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT((testResponse.length() == 25) || (testResponse.length() == 26));
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
        verifySessionCMD(expectedReturn, 3);
        
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
            if (referenceFrontPanel&mask == mask) {
                if (reqData == 0x00) {
                    TS_ASSERT_EQUALS(testResponse.data()[4]&referenceFrontPanel, referenceFrontPanel&0xF0);
                } else TS_ASSERT_EQUALS(testResponse.data()[4]&referenceFrontPanel, referenceFrontPanel|reqData);
            } else
                TS_ASSERT_EQUALS(testResponse.data()[4], referenceFrontPanel);
        } else
            TS_WARN("Status CMD doesn't support Front Panel Status");
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
        verifySessionCMD(expectedReturn, 3);
    }
    
    //---------------------------------------------------
    //--------------- TEST Chassis Control --------------
    void testChassisControl_PowerOff(void) {
        TS_TRACE("Testing Chassis Control CMD with power off request -- 'not allowed in current state' error expected");
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x02;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x00;
        IpmiMessage request(blank_request, 22);        
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        
        TS_TRACE("\tChecking power off status");
        unsigned char expectedReturn[1] = {0x60};
        verifySessionCMD(expectedReturn, 1);
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
        unsigned char expectedReturn[3] = {0x61, 0x10, 0x00};
        verifySessionCMD(expectedReturn, 3);
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
       TS_FAIL("No timed identify -- TO DO: check 15 sec identify interval");
   }
    
    void testChassisIdentify_IntervalArg(void){
        TS_TRACE("Testing Chassis Identify CMD with (optional) interval request byte");
        unsigned char reqData[1] = {0x01};
        verifyIdentifyCMD(reqData, 1, 0x00,  0x00); // Change mask & exp. value if implementing timed identify
        TS_FAIL("No timed identify -- TO DO: check 15 sec identify interval");
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
        verifyFrontPanelCMD(0x08, getReferenceFrontPanel());
    }
    
    void testChassisFrontPanel_disableDiagnostic(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable diagnostic request");
        verifyFrontPanelCMD(0x04, getReferenceFrontPanel());
    }
    void testChassisFrontPanel_disableDiagnosticAndStandby(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable diagnostic & standby request");
        verifyFrontPanelCMD(0x0C, getReferenceFrontPanel());
    }
    void testChassisFrontPanel_disableReset(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable reset request");
        verifyFrontPanelCMD(0x02, getReferenceFrontPanel());
    }
    void testChassisFrontPanel_disableDiagnosticAndReset(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable diagnostic & reset request");
        verifyFrontPanelCMD(0x06, getReferenceFrontPanel());
    }
    void testChassisFrontPanel_disableResetAndStandby(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable reset & standby request");
        verifyFrontPanelCMD(0x0A, getReferenceFrontPanel());
    }
    void testChassisFrontPanel_disableDiagnosticResetStandby(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable diagnostic, reset & standby request");
        verifyFrontPanelCMD(0x0D, getReferenceFrontPanel());
    }
    void testChassisFrontPanel_disablePowerOff(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable power off request");
        verifyFrontPanelCMD(0x01, getReferenceFrontPanel());
    }
    void testChassisFrontPanel_disableStandbyAndPowerOff(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable standby & power off request");
        verifyFrontPanelCMD(0x09, getReferenceFrontPanel());
    }
    void testChassisFrontPanel_disableDiagnosticAndPowerOff(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable diagnostic & power off request");
        verifyFrontPanelCMD(0x05, getReferenceFrontPanel());
    }
    void testChassisFrontPanel_disableResetAndPowerOff(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable reset & power off request");
        verifyFrontPanelCMD(0x05, getReferenceFrontPanel());
    }
    void testChassisFrontPanel_disableAll(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with disable all buttons request");
        verifyFrontPanelCMD(0x0F, getReferenceFrontPanel());
    }
    void testChassisFrontPanel_enableAll(void){
        TS_TRACE("Testing Set Front Panel Enables CMD with enable all buttons request");
        verifyFrontPanelCMD(0x00, getReferenceFrontPanel());
    }
    
    //---------------------------------------------------
    //---------- TEST Set Chassis Capabilities ----------
    
    
    //---------------------------------------------------
    //------ TEST Set Chassis Power Restore Policy ------
    
    
    //---------------------------------------------------
    //------ TEST Set Chassis Power Restore Policy ------
    
    
    //---------------------------------------------------
    //------ TEST Get Chassis System Restart Cause ------
    
    
    //---------------------------------------------------
    //---------- TEST Set Chassis POH Counter -----------
    
    
};


#endif
