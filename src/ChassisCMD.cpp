#include <MsgHandler.h>
#include <ChassisCMD.h>
#include <IpmiCommandDefines.h>
#include <fstream>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;
extern cmdMap cmds;


int  GetChassisCapabCMD::process( const unsigned char* request, unsigned char* response ){
    log_file << "Get Chassis Capabilities Command" << std::endl;
    
    response[0] = COMP_CODE_OK;
    response[1] = capabFlag;
    response[2] = fruAddress;
    response[3] = sdrAddress;
    response[4] = selAddress;
    response[5] = sysMgmtAddress;

    return 6;
    
}

GetChassisStatusCMD::GetChassisStatusCMD() : curPowerState(0x61), lastPowerEvent(0x00) { }

void GetChassisStatusCMD::setLastPowerEvent(unsigned char eventByte) {
    lastPowerEvent = eventByte;
}

void GetChassisStatusCMD::setPowerState(unsigned char powerMask) {
    curPowerState = curPowerState && powerMask;
}

int  GetChassisStatusCMD::process( const unsigned char* request, unsigned char* response ){
    if (request[0] == 0xFF ) setLastPowerEvent((int)response[0]);
    else if (request[0] == 0xFE ) setPowerState((int)response[0]);
    else {
        log_file << "Get Chassis Status Command" << std::endl;
    
        response[0] = COMP_CODE_OK;
        response[1] = curPowerState;
        response[2] = lastPowerEvent;
        response[3] = miscChassisState;
    
        return 4;
    }
}

int  ChassisCntrlCMD::process( const unsigned char* request, unsigned char* response ){
    log_file << "Chassis Control Command: " << std::flush;
    if (request[DATA_START_INDEX] == 0x00) {
        log_file << " power off " << std::endl;
        cmds[0x01]->process(new unsigned char (0xFE), new unsigned char (0x1E));
    } else if (request[DATA_START_INDEX] == 0x01) {
        log_file << " power on " << std::endl;
       cmds[0x01]->process(new unsigned char (0xFF), new unsigned char (0x10));
    }
    
    response[0] = COMP_CODE_OK;
    
    return 1;
}

int  ChassisResetCMD::process( const unsigned char* request, unsigned char* response ){
    response[0] = COMP_CODE_OK;
    
    return 1;
}


int  ChassisIdentifyCMD::process( const unsigned char* request, unsigned char* response ){
    response[0] = COMP_CODE_OK;
    
    return 1;
}