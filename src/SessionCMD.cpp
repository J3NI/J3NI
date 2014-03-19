#include <SessionCMD.h>
#include <ChannelCMD.h>
#include <IpmiCommandDefines.h>
#include <IpmiMessage.h>
#include <fstream>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

GetSessionChalCMD::GetSessionChalCMD(const char* userName)
    :   userName_(NULL)
{
    if(userName != NULL)
    {
        userName_ = new unsigned char[16];
        for(int i = 0; i < 16; i++)
        {
            userName_[i] = 0x00;
        }
        for(int i = 0; (i < 16) && (userName[i] != 0x00); i++)
        {
            userName_[i] = userName[i];
        }
    }
}

GetSessionChalCMD::~GetSessionChalCMD()
{
    if(userName_ != NULL) delete[] userName_;
}

unsigned char GetSessionChalCMD::verifyUserName(const unsigned char* uname) const
{
    if(userName_ == NULL) return COMP_CODE_OK;
    if(uname[0] == 0x00)
    {
        return NULL_USER_NAME;
    }
    for(int i = 0; i < 16; i++)
    {
        if(uname[i] != userName_[i])
        {
            return INVALID_USER_NAME;
        }
    }
    return COMP_CODE_OK;
}

int GetSessionChalCMD::process(const unsigned char* request, int reqLength, unsigned char* response){
    log_file << "Processing Get Session Challenge CMD" << std::endl;
    
    response[0] = COMP_CODE_OK;
    if(userName_ != NULL && request[0] == 0x04 && reqLength >= 17)
    {
        response[0] = verifyUserName(request + 1);
        if(response[0] != COMP_CODE_OK)
        {
            return 1;
        }
    }
    
    IpmiMessage::setSessionId(TEMP_SESSION_ID, 4);
    for (int i = 0; i < 4; i++) {
        response[i+1] = TEMP_SESSION_ID[i];
    }
    
    for (int i = 0; i < 16; i++) {
        response[i+5] = CHALLENGE_STRING[i];
    }
    
    return 21;
}

ActSessionCMD::ActSessionCMD(SetSessionPrivCMD* priv): privCMD(priv), authType(0x00) { }

bool ActSessionCMD::verifyTempID(const unsigned char* data){
    for(int i = 0; i < 4; i++) {
        if (data[SESSION_ID_INDEX+i] != TEMP_SESSION_ID[i]) return false;
    }
    return true;
}

bool ActSessionCMD::verifyChalString(const unsigned char* data){
    int authDataSize = 0;
    if(data[AUTH_TYPE_INDEX] != 0x0) authDataSize = 16;
    for(int i = 0; i < 16; i++) {
        if (data[DATA_START_INDEX+i+2+authDataSize] != CHALLENGE_STRING[i]) return false;
    }
    return true;
}

int ActSessionCMD::process(const unsigned char* request, int reqLength, unsigned char* response){
    log_file << "Processing Activate Session CMD" << std::endl;
    if (!verifyTempID(request)) response[0] = INVALID_SESSION_ID;
    else if (!verifyChalString(request) ) response[0] = UNKNOWN_ERROR;
    else{
        response[0] = COMP_CODE_OK;
        response[1] = authType;
        for (int i = 0; i < 4; i++) response[i+2] = request[DATA_START_INDEX + i+19];
        // response[6:9] all 0s for single session connection
        response[10] = privCMD->getPrivLvl();
        
        IpmiMessage::setSessionId(response + 2, 4);
        return 11;
    }
    return 1;
}

SetSessionPrivCMD::SetSessionPrivCMD(): privLvl(0x04){}

int SetSessionPrivCMD::process(const unsigned char* request, int reqLength, unsigned char* response){
    log_file << "Processing Set Session Privilege Level CMD" << std::endl;
    if (setPrivLvl(request[0]) || request[0] == 0x01) {
        response[0] =  COMP_CODE_OK ;
        response[1] =  privLvl;
        return 2;
    }
    response[0] = PRIV_LVL_UNAVAILABLE;
    return 1;
}

unsigned char SetSessionPrivCMD::getPrivLvl(){
    return privLvl;
}

bool SetSessionPrivCMD::setPrivLvl(const unsigned char newLvl){
    if ((newLvl >= 2) && (newLvl <= 4)) {
        privLvl = newLvl;
        return true;
    }
    return false;
}

int CloseSessionCMD::process(const unsigned char* request, int reqLength, unsigned char* response){
    log_file << "Closing session" << std::endl;
    response[0] = COMP_CODE_OK;
    return 1;
}

GetSessionInfo::GetSessionInfo(SetSessionPrivCMD* priv, GetChannelAuthCMD* channel): privCMD(priv),channelCMD(channel){ }

int GetSessionInfo::process(const unsigned char* request, int reqLength, unsigned char* response){
    response[0] = COMP_CODE_OK;
    response[1] = request[1];               //Session handle
    response[2] = 0x01;                     //Number of possible active sessions: 1
    response[3] = 0x01;                     //Number of currently active sessions: 1
    response[4] = 0x01;                     //User ID <------ PROBABLY NOT STATIC; NEEDS TO CHANGE
    response[5] = privCMD->getPrivLvl();    //Privilege Level
    response[6] = 0x10 | channelCMD->getChannelNum() ;
    return 7;
}
