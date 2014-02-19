#ifndef CHASSISCMD_H
#define CHASSISCMD_H

#include <I_Command.h>

class  GetChassisCapabCMD:public I_Command{
private:
    // Capabilities Flag [7 - 4] = 0x0 (reserved)
    // [3] = 1 ( power interlock )
    // [2] = 1 ( diagnostic interrupt )
    // [1] = 0 ( front panel lockout )
    // [0] = 0 ( intrusion sensor )
    const static unsigned char capabFlag = 0x0B;
    
    const static unsigned char fruAddress = 0x00;// unspecified
    const static unsigned char sdrAddress = 0x00;
    const static unsigned char selAddress = 0x00;
    const static unsigned char sysMgmtAddress = 0x00;
    // Chassis Bridge address not provided - will assume BMC address
    
public:
    int process( const unsigned char* request, unsigned char* response );
};

class  GetChassisStatusCMD:public I_Command{
private:
    //Current Power State [7] - reserved
    // [6 - 5] = 11 (unknown)
    // [4] = 0 (power control fault)
    // [3] = 0 (power fault)
    // [2] = 0 (power interlock)
    // [1] = 0 (power overload)
    // [0] = 1 (power on)
    unsigned char curPowerState;
    
    //Last Power Event [7 - 5] - reserved
    // [4] = 0 (last power on via IPMI command)
    // [3] = 0 (last power down via power fault)
    // [2] = 0 (last power down via power interlock)
    // [1] = 0 (last power down via power overload)
    // [0] = 0 (AC failed)
    unsigned char lastPowerEvent;
    
    //Misc. Chassis State [7] - reserved
    // [6] = 0 (Chassis Identify command supported)
    // [5 - 4] = 0x00 (power control fault)
    // [3] = 0 (fan fault)
    // [2] = 0 (drive fault)
    // [1] = 0 (front panel lockout active)
    // [0] = 0 (chassis intrusion active)
    const static unsigned char miscChassisState = 0x00;
    
    // Front Panel Button Capabilities not supported
    
    
public:
    GetChassisStatusCMD();
    void setPowerState(unsigned char powerMask);
    void setLastPowerEvent(unsigned char eventByte);
    int process( const unsigned char* request, unsigned char* response );
};

class  ChassisCntrlCMD:public I_Command{
private:
    GetChassisStatusCMD* statusCmd_;
    
public:
    ChassisCntrlCMD(GetChassisStatusCMD* chassisStatusCmd);
    int process( const unsigned char* request, unsigned char* response );
};

class  ChassisResetCMD:public I_Command{
public:
    int process( const unsigned char* request, unsigned char* response );
};

class  ChassisIdentifyCMD:public I_Command{
public:
    int process( const unsigned char* request, unsigned char* response );
};

#endif