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

    // Test Activate Session Command

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