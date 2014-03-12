#include <MsgHandler.h>
#include <IpmiMessage.h>
#include <IpmiCommandDefines.h>

#include <ChassisCMD.h>
#include <ChannelCMD.h>
#include <SessionCMD.h>
#include <SoLCMD.h>
#include <LANCMD.h>

#include <fstream>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

MsgHandler::CommandMap MsgHandler::commands_;

void MsgHandler::initCMD() {
    // Chassis Commands
    GetChassisCapabCMD* chassisCapab = new GetChassisCapabCMD();
    GetChassisStatusCMD* chassisStatus = new GetChassisStatusCMD();
    GetChassisRestartCause* chassisRestartCause = new GetChassisRestartCause();
    GetChassisPOHCounter* chassisPOH = new GetChassisPOHCounter();
    
    commands_[0x00] = chassisCapab;
    commands_[0x01] = chassisStatus;
    commands_[0x02] = new ChassisCntrlCMD(chassisStatus, chassisRestartCause, chassisPOH);
    commands_[0x03] = new ChassisResetCMD();
    commands_[0x04] = new ChassisIdentifyCMD();
    commands_[0x0A] = new ChassisFrontPanelCMD(chassisStatus);
    commands_[0x05] = new SetChassisCapabCMD(chassisCapab);
    commands_[0x06] = new SetChassisPowerRestore(chassisStatus);
    commands_[0x0B] = new SetChassisPowerCycle();
    commands_[0x07] = chassisRestartCause;
    /*
    commands_[0x08] = new SetChassisBootOpt();
    commands_[0x09] = new GetChassisBootOpt();
         */
    commands_[0x0F] = chassisPOH;

    
    // Channel Commands
    GetChannelAuthCMD* channelAuthCMD = new GetChannelAuthCMD();
    commands_[0x38] = channelAuthCMD;
    
    //Session Commands
    SetSessionPrivCMD* sessionPrivCMD = new SetSessionPrivCMD();
    commands_[0x39] = new GetSessionChalCMD();
    commands_[0x3a] = new ActSessionCMD(sessionPrivCMD);
    commands_[0x3b] = sessionPrivCMD;
    commands_[0x3c] = new CloseSessionCMD();
    commands_[0x3d] = new GetSessionInfo(sessionPrivCMD, channelAuthCMD);

    //SoL Commands
    GetSoLConfigCMD * solConfig = new GetSoLConfigCMD();
    commands_[0x20] = new SoLActivatingCMD();
    commands_[0x21] = new SetSoLConfigCMD(solConfig);
    commands_[0x22] = solConfig;
    
    //LAN Commands
    GetLANConfigCMD * LANConfig = new GetLANConfigCMD();
    commands_[0x01] = new SetLANConfigCMD(LANConfig);
    commands_[0x02] = LANConfig;
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
        if (message[COMMAND_INDEX] == 0x3a) respLen = commands_[0x3a]->process(message.message(), message.length(), respData);
        else respLen = commands_[message[COMMAND_INDEX]]->process(message.data(), message.dataLength(), respData);
    } else {
        respData[0] = UNKNOWN_ERROR;
    }
    message.serialize(respData, respLen, response);
}




