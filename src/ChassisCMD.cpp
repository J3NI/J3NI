#include <MsgHandler.h>
#include <ChassisCMD.h>
#include <IpmiCommandDefines.h>
#include <fstream>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;
<<<<<<< HEAD
extern cmdMap cmds;
=======
>>>>>>> 51eff7ee8b5be3140c68d52696d8ada92d509400

int  GetChassisCapabCMD::process( const unsigned char* request, unsigned char* response ){
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

GetChassisStatusCMD::GetChassisStatusCMD() : curPowerState(0x61), lastPowerEvent(0x00) { }

void GetChassisStatusCMD::setLastPowerEvent(unsigned char eventByte) {
    lastPowerEvent = eventByte;
}

void GetChassisStatusCMD::setPowerState(unsigned char powerState) {
    curPowerState = powerState;
}

int  GetChassisStatusCMD::process( const unsigned char* request, unsigned char* response ){
    if (response == NULL) {
        if (!request) return (int)curPowerState && 1;
        setLastPowerEvent((int)request[0]);
        setPowerState((int)request[1]);
    } else {
        log_file << "Get Chassis Status Command" << std::endl;
    
        response[0] = COMP_CODE_OK;
        response[1] = curPowerState;
        response[2] = lastPowerEvent;
        response[3] = miscChassisState;
    
        return 4;
    }
    response[0] = COMP_CODE_OK;
    return 1;
}

ChassisCntrlCMD::ChassisCntrlCMD(GetChassisStatusCMD* chassisStatusCmd)
{
    statusCmd_ = chassisStatusCmd;
}


int ChassisCntrlCMD::process( const unsigned char* request, unsigned char* response ){
    log_file << "Chassis Control Command: " << std::flush;
    response[0] = COMP_CODE_OK;
    
    if (request[DATA_START_INDEX] == 0x00 || request[DATA_START_INDEX] == 0x05 ) {
        log_file << " power off " << std::endl;
<<<<<<< HEAD
        cmds[0x01]->process(new unsigned char [0x00, 0x60], NULL);
    } else if (request[DATA_START_INDEX] == 0x01) {
        log_file << " power on " << std::endl;
        cmds[0x01]->process(new unsigned char [0x10, 0x61], NULL);
    } else if (request[DATA_START_INDEX] == 0x02 || request[DATA_START_INDEX] == 0x03 ) {
        if ( !cmds[0x01]->process(NULL, NULL) ) response[0] = CANNOT_EXEC_IN_CUR_STATE;
        else cmds[0x01]->process(new unsigned char [0x00, 0x61], NULL);
=======
        statusCmd_->setPowerState(0x1E);
    } else if (request[DATA_START_INDEX] == 0x01) {
        log_file << " power on " << std::endl;
        statusCmd_->setLastPowerEvent(0x10);
>>>>>>> 51eff7ee8b5be3140c68d52696d8ada92d509400
    }
    return 1;
}

<<<<<<< HEAD
int  ChassisResetCMD::process( const unsigned char* request, unsigned char* response ){
    response[0] = CANNOT_EXEC_DISABLED;
=======
int ChassisResetCMD::process( const unsigned char* request, unsigned char* response ){
    response[0] = COMP_CODE_OK;
    
>>>>>>> 51eff7ee8b5be3140c68d52696d8ada92d509400
    return 1;
}


int  ChassisIdentifyCMD::process( const unsigned char* request, unsigned char* response ){
    response[0] = COMP_CODE_OK;
    return 1;
}
