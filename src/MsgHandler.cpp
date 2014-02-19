#include <MsgHandler.h>
#include <IpmiMessage.h>
#include <IpmiCommandDefines.h>

#include <ChassisCMD.h>
#include <ChannelCMD.h>
#include <SessionCMD.h>

#include <fstream>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

MsgHandler::CommandMap MsgHandler::commands_;

void MsgHandler::initCMD() {
    // Chassis Commands
    commands_[0x00] = new GetChassisCapabCMD();
    GetChassisStatusCMD* chassisStatus = new GetChassisStatusCMD();
    commands_[0x01] = chassisStatus;
    commands_[0x02] = new ChassisCntrlCMD(chassisStatus);
    commands_[0x03] = new ChassisResetCMD();
    commands_[0x04] = new ChassisIdentifyCMD();
    
    // Channel Commands
    commands_[0x38] = new GetChannelAuthCMD();
    
    //Session Commands
    commands_[0x39] = new GetSessionChalCMD();
    commands_[0x3a] = new ActSessionCMD();
    commands_[0x3b] = new SetSessionPrivCMD();
    commands_[0x3c] = new CloseSessionCMD();
}

void MsgHandler::clearCMD()
{
    CommandMap::iterator it;
    for(it = commands_.begin(); it != commands_.end(); it++)
    {
        delete it->second;
    }
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
    unsigned char * respData = new unsigned char[MAX_DATA_SIZE];
    int respLen = 1;
    
    if  ( commands_.find(message[COMMAND_INDEX]) != commands_.end() ) {
        respLen = commands_[message[COMMAND_INDEX]]->process(message.message(), respData);
    } else {
        respData[0] = 0xFF;
    }
    message.serialize(respData, respLen, response);
}




