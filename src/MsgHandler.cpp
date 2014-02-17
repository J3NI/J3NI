#include <MsgHandler.h>
#include <IpmiMessage.h>
#include <IpmiCommandDefines.h>

#include <ChassisCMD.h>
#include <ChannelCMD.h>
#include <SessionCMD.h>

#include <fstream>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;
extern cmdMap cmds;

void MsgHandler::initCMD() {
    cmds[0x38] = new GetChannelAuthCMD();
    cmds[0x39] = new GetSessionChalCMD();
    cmds[0x3a] = new ActSessionCMD();
    cmds[0x3b] = new SetSessionPrivCMD();
    cmds[0x3c] = new CloseSessionCMD();
    
    cmds[0x00] = new GetChassisCapabCMD();
    cmds[0x01] = new GetChassisStatusCMD();
    cmds[0x02] = new ChassisCntrlCMD();
    cmds[0x03] = new ChassisResetCMD();
    cmds[0x04] = new ChassisIdentifyCMD();
}

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
    unsigned char * respData = new unsigned char[42];
    int respLen = 1;
    
    if  ( cmds.find(message[COMMAND_INDEX]) != cmds.end() ) {
        respLen = cmds[message[COMMAND_INDEX]]->process(message.message(), respData);
    } else {
        respData[0] = 0xFF;
    }
    message.serialize(respData, respLen, response);
}




