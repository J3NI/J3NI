#include <fstream>
#include <stdexcept>
#include <syslog.h>

#include <MsgHandler.h>
#include <ChassisCMD.h>
#include <IpmiCommandDefines.h>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

GetChassisCapabCMD::GetChassisCapabCMD(): fruAddress(0x00), sdrAddress(0x20), selAddress(0x20), sysMgmtAddress(0x20), bridgeAddress(0x00) { }

int  GetChassisCapabCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    syslog(LOG_NOTICE, "Processing Get Chassiz Capabilities CMD");
    log_file << "Get Chassis Capabilities Command" << std::endl;
    
    response[0] = COMP_CODE_OK;
    response[1] = capabFlag;
    response[2] = fruAddress;
    response[3] = sdrAddress;
    response[4] = selAddress;
    response[5] = sysMgmtAddress;
    
    if (bridgeAddress == 0) return 6;
    else {
        response[6] = bridgeAddress;
        return 7;
    }
    
}

unsigned char GetChassisCapabCMD::getCapabFlag(){
    return capabFlag;
}
unsigned char GetChassisCapabCMD::getFruAddress(){
    return fruAddress;
}
unsigned char GetChassisCapabCMD::getSdrAddress(){
    return sdrAddress;
}
unsigned char GetChassisCapabCMD::getSelAddress(){
    return selAddress;
}
unsigned char GetChassisCapabCMD::getSysMgmtAddress(){
    return sysMgmtAddress;
}
unsigned char GetChassisCapabCMD::getBridgeAddress(){
    return bridgeAddress;
}

void GetChassisCapabCMD::setAllFields(const unsigned char* data, int bridgeSet)
{
    if( bridgeSet == 5 || bridgeSet == 6){
        fruAddress = data[1];
        sdrAddress = data[2];
        selAddress = data[3];
        sysMgmtAddress = data[4];
        if (bridgeSet == 6) bridgeAddress = data[5];
    } else {
        syslog(LOG_ERR, "Recieved Set Chassis Capabilities CMD request provided less that 5 data bytes");
        log_file << "Recieved Set Chassis Capabilities CMD request provided less that 5 data bytes"<<std::endl;
    }

}

GetChassisStatusCMD::GetChassisStatusCMD() : curPowerState(0x61), lastPowerEvent(0x00), miscChassisState(0x00) { }

unsigned char GetChassisStatusCMD::getCurPowerState(){
    return curPowerState;
}
unsigned char GetChassisStatusCMD::getLastPowerEvent(){
    return lastPowerEvent;
}
unsigned char GetChassisStatusCMD::getMiscChassisState(){
    return miscChassisState;
}

void GetChassisStatusCMD::setLastPowerEvent(unsigned char eventByte)
{
    lastPowerEvent = eventByte;
}

void GetChassisStatusCMD::setPowerState(int powerState)
{
    if (powerState == 0) curPowerState = curPowerState & 0xFE;
    else curPowerState = curPowerState | 0x01;
}

void GetChassisStatusCMD::setPowerPolicy(unsigned char policy)
{
    unsigned char mask = ( policy << 5 ) | 0x9F;
    curPowerState = curPowerState & mask;
}

unsigned char GetChassisStatusCMD::getPowerPolicy()
{
    return (curPowerState >> 5) & 0x03;
}

void GetChassisStatusCMD::setMiscChassisState(unsigned char chassisState)
{
    miscChassisState = chassisState;
}

int GetChassisStatusCMD::getCurrentPower()
{
    return (int)(curPowerState & 0x01);
}

int  GetChassisStatusCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    syslog(LOG_NOTICE, "Processing Get Chassis Status CMD");
    log_file << "Get Chassis Status Command" << std::endl;
    
    response[0] = COMP_CODE_OK;
    response[1] = curPowerState;
    response[2] = lastPowerEvent;
    response[3] = miscChassisState;
    
    return 4;
}

ChassisCntrlCMD::ChassisCntrlCMD(GetChassisStatusCMD* chassisStatusCmd, GetChassisRestartCause* restartCause, GetChassisPOHCounter* pohCmd)
{
    statusCmd_ = chassisStatusCmd;
    restartCause_ = restartCause;
    poh_ = pohCmd;
}


int ChassisCntrlCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    const char* logMessage = "Processing Chassis Control CMD%s";
    const char* powerState = "";
    
    log_file << "Chassis Control Command: " << std::flush;
    response[0] = COMP_CODE_OK;
    
    if (request[0] == 0x00 || request[0] == 0x05 ) {
        powerState = ": power off";
        syslog(LOG_NOTICE, logMessage, powerState);
        log_file << " power off " << std::endl;
        statusCmd_->setPowerState(0);
    } else if (request[0] == 0x01) {
        powerState = ": power on";
        syslog(LOG_NOTICE, logMessage, powerState);
        log_file << " power on " << std::endl;
        statusCmd_->setLastPowerEvent(0x10);
        statusCmd_->setPowerState(1);

        poh_->resetStartTime();
    } else if (request[0] == 0x02 || request[0] == 0x03 ) {
        syslog(LOG_NOTICE, logMessage, powerState);
        if ( !statusCmd_->getCurrentPower() ) response[0] = CANNOT_EXEC_IN_CUR_STATE;
        else{
            statusCmd_->setPowerState(1);
            statusCmd_->setLastPowerEvent(0x00);
            restartCause_->setRestartCause(0x01);
            poh_->resetStartTime();
        }
    }
    return 1;
}

int  ChassisResetCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    response[0] = CANNOT_EXEC_DISABLED;
    return 1;
}


int  ChassisIdentifyCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    response[0] = COMP_CODE_OK;
    return 1;
}

ChassisFrontPanelCMD::ChassisFrontPanelCMD(GetChassisStatusCMD* chassisStatusCmd)
{
    statusCmd_ = chassisStatusCmd;
}

int ChassisFrontPanelCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    if (request[0] == 0x00 || request[0] == 0x01)
        statusCmd_->setMiscChassisState(0x02);
    response[0] = COMP_CODE_OK;
    return 1;
}


SetChassisCapabCMD::SetChassisCapabCMD(GetChassisCapabCMD* chassisCapabCmd)
{
    capabCmd_ = chassisCapabCmd;
}

int SetChassisCapabCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    capabCmd_->setAllFields(request, reqLength );
    response[0] = COMP_CODE_OK;
    return 1;
}

SetChassisPowerRestore::SetChassisPowerRestore(GetChassisStatusCMD* chassisStatusCmd)
{
    statusCmd_ = chassisStatusCmd;
}

int SetChassisPowerRestore::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    statusCmd_->setPowerPolicy(request[0]);
    response[0] = COMP_CODE_OK;
    response[1] = statusCmd_->getPowerPolicy();
    return 2;
}

int  SetChassisPowerCycle::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    response[0] = COMP_CODE_OK;
    return 1;
}

GetChassisRestartCause::GetChassisRestartCause():restartCause(0x00){ }

unsigned char GetChassisRestartCause::getRestartCause(){
    return restartCause;
}

void GetChassisRestartCause::setRestartCause(unsigned char cause){
    restartCause = cause;
}

int  GetChassisRestartCause::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    response[0] = COMP_CODE_OK;
    response[1] = restartCause;
    response[2] = 0x01;
    return 3;
}

GetChassisPOHCounter::GetChassisPOHCounter()
{
    struct tm initialStartTime;
    initialStartTime.tm_hour = 0; initialStartTime.tm_min = 0; initialStartTime.tm_sec = 0;
    initialStartTime.tm_year = 114; initialStartTime.tm_mon = 0; initialStartTime.tm_mday = 1;
    startTime = mktime(&initialStartTime);
    
}

int  GetChassisPOHCounter::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    response[0] = COMP_CODE_OK;
    response[1] = 0x3c; // 60 minutes per count
    int countNum = difftime( time(0), startTime) / 3600;
    for (int i = 0; i< 4; i++)
        response[2+i] = (countNum >> ((i)*8)) & 0xFF;
    return 6;
}

void  GetChassisPOHCounter::resetStartTime()
{
    startTime = time(0);
}
