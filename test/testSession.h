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
        blank_request = new unsigned char[IpmiCommandDefines::MAX_DATA_SIZE+IpmiCommandDefines::AUTH_CODE_LENGTH];
        blank_request[IpmiCommandDefines::NET_FN_INDEX] = 0x18;
    }
    
    void tearDown() {
        MsgHandler::clearCMD();
        delete blank_request;
    }
    
    void checkChallenge_auth(unsigned char* name, int nLen, unsigned char* pswd, int pLen,  unsigned char code, int len ){
        int authSize = 0;
        if (pswd) {
            authSize = IpmiCommandDefines::AUTH_CODE_LENGTH;
            blank_request[IpmiCommandDefines::AUTH_TYPE_INDEX] = 0x04;
        }
        if (name) blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x04; // Username/Password Auth
        for (int i = 0; i < nLen; i++) blank_request[IpmiCommandDefines::DATA_START_INDEX+i+1+authSize] = name[i];
        blank_request[IpmiCommandDefines::DATA_START_INDEX+authSize] = 0x04;
        blank_request[IpmiCommandDefines::COMMAND_INDEX+authSize] = 0x39;
        blank_request[IpmiCommandDefines::NET_FN_INDEX+authSize] = 0x18;
        for (int i = pLen; i < authSize-pLen; i++) blank_request[IpmiCommandDefines::AUTH_CODE_INDEX+i] = 0x00;
        for (int i = 0; i < pLen; i++) blank_request[IpmiCommandDefines::AUTH_CODE_INDEX+i] = pswd[i];

        IpmiMessage request(blank_request, 38+pLen);
        IpmiMessage testResponse;
        
        MsgHandler::processRequest(request, testResponse);
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX+authSize], code);
        TS_ASSERT_EQUALS(testResponse.length(), len+authSize);
        if (len == 42){
            for (int i = 0; i < 4; i++) TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX+i+authSize+1], IpmiCommandDefines::TEMP_SESSION_ID[i]);
            for (int i = 0; i < 16; i++) TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX+i+authSize+5], IpmiCommandDefines::CHALLENGE_STRING[i])
        }
    }
    
    // Test Get Session Challenge Command
    void testGetSessionChallenge_NoAuth(void) {
        TS_TRACE("Testing Get Session Challenge Command with None authentication and Null username");
        checkChallenge_auth(NULL, 0, NULL, 0, IpmiCommandDefines::COMP_CODE_OK, 42);
    }
    
    void testGetSessionChallenge_NoAuth_Name(void) {
        TS_TRACE("Testing Get Session Challenge Command without authentication, but a name supplied");
        unsigned char name[3] = {0x41, 0x6e, 0x6e};
        checkChallenge_auth(name, 3, NULL, 0, IpmiCommandDefines::COMP_CODE_OK, 42);
    }
    
    void testGetSessionChallenge_Auth_Name(void) {
        MsgHandler::clearCMD();
        MsgHandler::initCMD("ANN");
        TS_TRACE("Testing Get Session Challenge Command with user name authentication, but wrong name supplied");
        unsigned char name[3] = {0x41, 0x6e, 0x6e};
        checkChallenge_auth(name, 3, NULL, 0, IpmiCommandDefines::INVALID_USER_NAME, 22);
    }
    
    void testGetSessionChallenge_Auth_NoPswd(void) {
        IpmiMessage::setPassword("Pa5Sw0RD");
        MsgHandler::clearCMD();
        MsgHandler::initCMD("ANN");
        TS_TRACE("Testing Get Session Challenge Command with user name authentication,name but no password supplied");
        unsigned char name[3] = {0x41, 0x4E, 0x4E};
        checkChallenge_auth(name, 3, NULL, 0, IpmiCommandDefines::INVALID_USER_NAME, 22);
    }
    
    void testGetSessionChallenge_Auth_Pswd(void) {
        IpmiMessage::setPassword("Pa5Sw0RD");
        MsgHandler::clearCMD();     
        MsgHandler::initCMD("ANN");
        TS_TRACE("Testing Get Session Challenge Command with user name authentication,name and password supplied");
        unsigned char name[3] = {0x41, 0x4E, 0x4E};
        unsigned char pswd[8] = {0x50, 0x61, 0x35, 0x53, 0x77, 0x30, 0x52, 0x44};
        checkChallenge_auth(name, 3, pswd, 8, IpmiCommandDefines::COMP_CODE_OK, 42);
    }
    
    // Test Set Session Privilege Level
    void testSetSessionPrivilege(void) {
        TS_TRACE("Testing Set Session Privilege Level Command");
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x3b;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x04;
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
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x05;
        IpmiMessage request(blank_request, 22);
        IpmiMessage testResponse;
        MsgHandler::processRequest(request, testResponse);
        
        TS_ASSERT_EQUALS(testResponse[IpmiCommandDefines::DATA_START_INDEX], 0x80);
        TS_ASSERT_EQUALS(testResponse.length(), 22);
    }
    void testSetSessionPrivilege_Invalid(void) {
        TS_TRACE("Testing Set Session Privilege Level Command");
        
        blank_request[IpmiCommandDefines::COMMAND_INDEX] = 0x3b;
        blank_request[IpmiCommandDefines::DATA_START_INDEX] = 0x00;
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
