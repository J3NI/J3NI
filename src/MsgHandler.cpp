#include <MsgHandler.h>
#include <IpmiMessage.h>
#include <IpmiCommandDefines.h>
#include <fstream>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;


bool MsgHandler::isPing(const IpmiMessage& message)
{
    if(message.length() != PING_LENGTH)
    {
        return false;
    }
    int n = message.length();
    while (--n >= 0 && (n==9 || message[n] == PING_REQUEST[n]));
    return n != 0;
}

void MsgHandler::pong(const IpmiMessage& message, IpmiMessage& response)
{
    response.setMessage(PONG_RESPONSE, PONG_LENGTH);
    response[9] = message[9];
}

void MsgHandler::processRequest(const IpmiMessage& message,
                                IpmiMessage& response)
{
    cmdMap cmds;
    cmds[0x38] = new GetChannelAuthCMD();
    cmds[0x39] = new GetSessionChalCMD();
    cmds[0x3a] = new ActSessionCMD();
    cmds[0x3b] = new SetSessionPrivCMD();
    cmds[0x3c] = new CloseSessionCMD();
    
    unsigned char * respData = new unsigned char[42];
    int respLen = 1;
    
    if  ( cmds.find(message[COMMAND_INDEX]) != cmds.end() ) {
        respLen = cmds[message[COMMAND_INDEX]]->process(message.data(), respData);
    } else {
        respData[0] = 0xFF;
    }
    message.serialize(respData, respLen, response);
}

 int GetChannelAuthCMD::process(const unsigned char* request, unsigned char* response)
{
    log_file << "In GetChannelAuthCMD" << std::endl;
 
    response[0] = COMP_CODE_OK;
    response[1] = 0x01; //Channel Number
    response[2] = 0x01; //Authentication supported = None
    response[3] = 0x1b;
    
    return 9;
}
 int GetSessionChalCMD::process(const unsigned char* request, unsigned char* response){
    log_file << "In GetSessionChalCMD" << std::endl;

    response[0] =  COMP_CODE_OK ;
    // [2:5] is temporary session id - not sure if needed for single session operation
    unsigned char challengeString[16] = {   0x00, 0x01, 0x02, 0x03, 0x04,
                                            0x05, 0x06, 0x07, 0x08, 0x09,
                                            0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    for (int i = 0; i < 16; i++) {
        response[i+6] = challengeString[i];
    }
    
    return 21;
}
 int ActSessionCMD::process(const unsigned char* request, unsigned char* response){
    log_file << "In ActSessionCMD" << std::endl;
    for (int i = 0; i <4; i++) response[i+2] = request[i+19];
    response[10] = 0x04;
    return 11;
}
 int SetSessionPrivCMD::process(const unsigned char* request, unsigned char* response){
    log_file << "In SetSessionPrivCMD" << std::endl;
    response[0] =  0x00 ;
    response[1] =  request[0] ;
    return 2;
}
 int CloseSessionCMD::process(const unsigned char* request, unsigned char* response){
    log_file << "In CloseSessionCMD" << std::endl;
    return 1;
}

