#ifndef __TEST_SESSION_H
#define __TEST_SESSION_H

class TestSessionSuite : public CxxTest::TestSuite
{
    
private:
    unsigned char* blank_request;
    
public:
    
    static TestSessionSuite* createSuite() {

        return new TestSessionSuite();
    }
    
    static void destroySuite(TestSessionSuite* suite) {

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
    
    // Test Get Session Challenge Command
    void testGetSessionChallenge_NoneAuth(void) {
        TS_TRACE("Testing Get Session Challenge Command with None authentication and Null username");
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x39;
        IpmiMessage request(blank_request, 38);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 42);
    }
    void testGetSessionChallenge_Auth_WrongName(void) {
        TS_TRACE("Testing Get Session Challenge Command with pswd authentication requested");
        TS_WARN("THIS TEST will FAIL until authentication is implemented");
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x39;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x04;
        blank_request[IpmiCommandDefines::DATA_START_INDEX+1] = 0x41; //"A"
        blank_request[IpmiCommandDefines::DATA_START_INDEX+2] = 0x6e; //"n"
        blank_request[IpmiCommandDefines::DATA_START_INDEX+3] = 0x6e; //"n"

        IpmiMessage request(blank_request, 38);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], 0x81);
        TS_ASSERT_EQUALS(testResponse.length(), 22);

    }

    // Test Set Session Privilege Level
    void testSetSessionPrivilege(void) {
        TS_TRACE("Testing Set Session Privilege Level Command");
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x3b;
        blank_request[IpmiCommandDefines::DATA_START_INDEX+1] = 0x04;
        IpmiMessage request(blank_request, 22);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX+1], 0x04);
        TS_ASSERT_EQUALS(testResponse.length(), 23);
    }
    void testSetSessionPrivilege_Exceeding(void) {
        TS_TRACE("Testing Set Session Privilege Level Command");
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x3b;
        blank_request[IpmiCommandDefines::DATA_START_INDEX+1] = 0x05;
        IpmiMessage request(blank_request, 22);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], 0x80);
        TS_ASSERT_EQUALS(testResponse.length(), 22);
    }
    void testSetSessionPrivilege_Invalid(void) {
        TS_TRACE("Testing Set Session Privilege Level Command");
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x3b;
        blank_request[IpmiCommandDefines::DATA_START_INDEX+1] = 0x00;
        IpmiMessage request(blank_request, 22);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], 0x80);
        TS_ASSERT_EQUALS(testResponse.length(), 22);
    }
    
    // Test Activate Session Command
    void testActivateSession_NoAuth(void){
        TS_TRACE("Testing session activation with No auth");
        TS_TRACE("Testing Set Session Privilege Level Command");
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x3a;
       for (int i = 0; i < 4; i++) blank_request[IpmiCommandDefines::SESSION_ID_INDEX+i] = IpmiCommandDefines::TEMP_SESSION_ID[i];
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x00;
        blank_request[IpmiCommandDefines::DATA_START_INDEX+1] = 0x04;
        for (int i = 0; i <16; i++) blank_request[IpmiCommandDefines::DATA_START_INDEX+2+i] = IpmiCommandDefines::CHALLENGE_STRING[i];
        blank_request[IpmiCommandDefines::DATA_START_INDEX+20] = 0xFF;
        IpmiMessage request(blank_request, 43);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], 0x00);
        TS_ASSERT_EQUALS(testResponse.length(), 32);
    }
     // TODO - Add Session test with Auth
         
    // Test Close Session Command
    void testCloseSession(void) {
        TS_TRACE("Testing close Session Command");
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x3c;
        IpmiMessage request(blank_request, 25);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);
        TS_ASSERT_EQUALS(testResponse.length(), 22);
    }
    // Test Get Session Info Command
    void testGetSessionInfo_NoData(void) {
        TS_TRACE("Testing Get Session Info Command");
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x3d;
        IpmiMessage request(blank_request, 22);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], IpmiCommandDefines::COMP_CODE_OK);

        TS_ASSERT_EQUALS(testResponse.length(), 28);
    }
};

#endif
