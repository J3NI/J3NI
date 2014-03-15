#include <BashScriptDefines.h>
#include <MsgHandler.h>
#include <IpmiMessage.h>
#include <IpmiCommandDefines.h>

#include <ChassisCMD.h>
#include <ChannelCMD.h>
#include <LANCMD.h>
#include <PefCMD.h>
#include <SessionCMD.h>
#include <SoLCMD.h>

#include <fstream>
#include <stdlib.h>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

MsgHandler::CommandMap MsgHandler::ChassisCommands_;
MsgHandler::CommandMap MsgHandler::BridgeCommands_;
MsgHandler::CommandMap MsgHandler::SECommands_;
MsgHandler::CommandMap MsgHandler::AppCommands_;
MsgHandler::CommandMap MsgHandler::FirmCommands_;
MsgHandler::CommandMap MsgHandler::StorageCommands_;
MsgHandler::CommandMap MsgHandler::TransportCommands_;

MsgHandler::bashMap MsgHandler::bashMap_;

bool MsgHandler::BashOK(unsigned char netFn, unsigned char  cmd){
    int bashMapIndex = (netFn<<8)|cmd;
    if  ( (bashMap_.find(bashMapIndex) != bashMap_.end()) && (bashMap_[bashMapIndex][0]!='\0')) {
        return system(bashMap_[bashMapIndex]);
    }
    return true;
}
void MsgHandler::initBash(){
    bashMap_[0x000] = bashSripts::GetChassisCapabilities;
    bashMap_[0x001] = bashSripts::GetChassisStatus;
    bashMap_[0x002] = bashSripts::ChassisControl;
    bashMap_[0x003] = bashSripts::ChassisReset;
    bashMap_[0x004] = bashSripts::ChassisIdentify;
    bashMap_[0x00A] = bashSripts::ChassisFrontPanel;
    bashMap_[0x005] = bashSripts::SetChassisCapabilities;
    bashMap_[0x006] = bashSripts::SetPowerRestorePolicy;
    bashMap_[0x00B] = bashSripts::SetChassisPowerCycle;
    bashMap_[0x007] = bashSripts::GetChassisRestartCause;
    bashMap_[0x00F] = bashSripts::GetPOH;
    
    bashMap_[0x410] = bashSripts::GetPEFCapabilities;
    bashMap_[0x411] = bashSripts::ArmPEFPostponeTimer;
    bashMap_[0x412] = bashSripts::SetPEFConfigurationParameters ;
    bashMap_[0x413] = bashSripts::GetPEFConfigurationParameters ;
    bashMap_[0x414] = bashSripts::SetLastProcessedEventID;
    bashMap_[0x415] = bashSripts::GetLastProcessedEventID;
    
    bashMap_[0x638] = bashSripts::GetChannelAuthentication;
    bashMap_[0x639] = bashSripts::GetSessionChallenge;
    bashMap_[0x63a] = bashSripts::ActivateSession;
    bashMap_[0x63b] = bashSripts::SetSessionPrivilegeLevel;
    bashMap_[0x63c] = bashSripts::CloseSession;
    bashMap_[0x63d] = bashSripts::GetSessionInfo;
    bashMap_[0x640] = bashSripts::SetChannelAccess;
    bashMap_[0x641] = bashSripts::GetChannelAccess;
    bashMap_[0x642] = bashSripts::GetChannelInfo;
    
    bashMap_[0xC01] = bashSripts::SetLANConfigurationParameters;
    bashMap_[0xC02] = bashSripts::GetLANConfigurationParameters;
    bashMap_[0xC20] = bashSripts::ActivateSOL;
    bashMap_[0xC21] = bashSripts::SetSOLConfigurationParameters;
    bashMap_[0xC22] = bashSripts::GetSOLConfigurationParameters;
    
}

void MsgHandler::initCMD() {
    initBash();
    // Chassis Commands
    GetChassisCapabCMD* chassisCapab = new GetChassisCapabCMD();
    GetChassisStatusCMD* chassisStatus = new GetChassisStatusCMD();
    GetChassisRestartCause* chassisRestartCause = new GetChassisRestartCause();
    GetChassisPOHCounter* chassisPOH = new GetChassisPOHCounter();
    
    ChassisCommands_[0x00] = chassisCapab;
    ChassisCommands_[0x01] = chassisStatus;
    ChassisCommands_[0x02] = new ChassisCntrlCMD(chassisStatus, chassisRestartCause, chassisPOH);
    ChassisCommands_[0x03] = new ChassisResetCMD();
    ChassisCommands_[0x04] = new ChassisIdentifyCMD();
    ChassisCommands_[0x0A] = new ChassisFrontPanelCMD(chassisStatus);
    ChassisCommands_[0x05] = new SetChassisCapabCMD(chassisCapab);
    ChassisCommands_[0x06] = new SetChassisPowerRestore(chassisStatus);
    ChassisCommands_[0x0B] = new SetChassisPowerCycle();
    ChassisCommands_[0x07] = chassisRestartCause;
    /*
    ChassisCommands_[0x08] = new SetChassisBootOpt();
    ChassisCommands_[0x09] = new GetChassisBootOpt();
         */
    ChassisCommands_[0x0F] = chassisPOH;
    
    // Channel Commands
    GetChannelAuthCMD* channelAuthCMD = new GetChannelAuthCMD();
    SetChannelAccessCMD* channelAccCMD = new SetChannelAccessCMD();
    AppCommands_[0x38] = channelAuthCMD;
    AppCommands_[0x42] = new GetChannelInfoCMD(channelAuthCMD);
    AppCommands_[0x40] = channelAccCMD;
    AppCommands_[0x41] = new GetChannelAccessCMD(channelAccCMD);

    //Session Commands
    SetSessionPrivCMD* sessionPrivCMD = new SetSessionPrivCMD();
    AppCommands_[0x39] = new GetSessionChalCMD();
    AppCommands_[0x3a] = new ActSessionCMD(sessionPrivCMD);
    AppCommands_[0x3b] = sessionPrivCMD;
    AppCommands_[0x3c] = new CloseSessionCMD();
    AppCommands_[0x3d] = new GetSessionInfo(sessionPrivCMD, channelAuthCMD);

    // PEF Commands
    GetPefConfigParamCMD* pefConfigParamCmd = new GetPefConfigParamCMD();
    GetLastProcEventIdCMD* procEventIdCmd = new GetLastProcEventIdCMD();
    SECommands_[0x10] = new GetPefCapabCMD();
    SECommands_[0x11] = new ArmPefPostponeTimerCMD();
    SECommands_[0x12] = new SetPefConfigParamCMD(pefConfigParamCmd);
    SECommands_[0x13] = new GetPefConfigParamCMD();
    SECommands_[0x14] = new SetLastProcEventIdCMD(procEventIdCmd);
    SECommands_[0x15] = new GetLastProcEventIdCMD();
    
    //SoL Commands
    GetSoLConfigCMD * solConfig = new GetSoLConfigCMD();
    TransportCommands_[0x20] = new SoLActivatingCMD();
    TransportCommands_[0x21] = new SetSoLConfigCMD(solConfig);
    TransportCommands_[0x22] = solConfig;
    
    //LAN Commands
    GetLANConfigCMD * LANConfig = new GetLANConfigCMD(channelAuthCMD);
    TransportCommands_[0x01] = new SetLANConfigCMD(LANConfig, channelAuthCMD);
    TransportCommands_[0x02] = LANConfig;
}

void MsgHandler::clearCMD()
{
    CommandMap::iterator it;
    for(it = ChassisCommands_.begin(); it != ChassisCommands_.end(); it++)
    {
        delete it->second;
    }
    ChassisCommands_.clear();
    
    for(it = BridgeCommands_.begin(); it != BridgeCommands_.end(); it++)
    {
        delete it->second;
    }
    BridgeCommands_.clear();
    
    for(it = SECommands_.begin(); it != SECommands_.end(); it++)
    {
        delete it->second;
    }
    SECommands_.clear();
    
    for(it = AppCommands_.begin(); it != AppCommands_.end(); it++)
    {
        delete it->second;
    }
    AppCommands_.clear();
    
    for(it = FirmCommands_.begin(); it != FirmCommands_.end(); it++)
    {
        delete it->second;
    }
    FirmCommands_.clear();
    
    for(it = StorageCommands_.begin(); it != StorageCommands_.end(); it++)
    {
        delete it->second;
    }
    StorageCommands_.clear();
    
    for(it = TransportCommands_.begin(); it != TransportCommands_.end(); it++)
    {
        delete it->second;
    }
    TransportCommands_.clear();
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
    for(int i = 0; i < MAX_DATA_SIZE; i++)
        respData[i] = 0x00;
    int respLen = 1;
    respData[0] = INVALID_CMD;
    
    unsigned char netFn = message.getNetFn();
    unsigned char cmd = message[COMMAND_INDEX];
    
    if (BashOK(netFn, cmd)){
    switch ( netFn ) {
        case 0x00:
            if  ( ChassisCommands_.find(cmd) != ChassisCommands_.end() ) {
                respLen = ChassisCommands_[cmd]->process(message.data(), message.dataLength(), respData);
            }
            break;

        case 0x02:
            if  ( BridgeCommands_.find(cmd) != BridgeCommands_.end() ) {
                respLen = BridgeCommands_[cmd]->process(message.data(), message.dataLength(), respData);
            }
            break;
        case 0x04:
            if  ( SECommands_.find(cmd) != SECommands_.end() ) {
                respLen = SECommands_[cmd]->process(message.data(), message.dataLength(), respData);
            }
            break;
        case 0x06:
            if  ( AppCommands_.find(cmd) != AppCommands_.end() ) {
                if (cmd == 0x3a)
                    respLen = AppCommands_[0x3a]->process(message.message(), message.length(), respData);
                else
                    respLen = AppCommands_[cmd]->process(message.data(), message.dataLength(), respData);
            }
            break;
        case 0x08:
            if  ( FirmCommands_.find(cmd) != FirmCommands_.end() ) {
                respLen = FirmCommands_[cmd]->process(message.data(), message.dataLength(), respData);
            }
            break;
        case 0x0A:
            if  ( StorageCommands_.find(cmd) != StorageCommands_.end() ) {
                respLen = StorageCommands_[cmd]->process(message.data(), message.dataLength(), respData);
            }
            break;
        case 0x0C:
            if  ( TransportCommands_.find(cmd) != TransportCommands_.end() ) {
                respLen = TransportCommands_[cmd]->process(message.data(), message.dataLength(), respData);
            }
            break;
        default:
             respData[0] = INVALID_CMD;
    }
    } else {
        respData[0] = UNKNOWN_ERROR;
    }
    message.serialize(respData, respLen, response);
}




