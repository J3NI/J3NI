#ifndef CHASSISCMD_H
#define CHASSISCMD_H

class cmdProcessor;

class  GetChassisCapabCMD:public cmdProcessor{
private:
    // Capabilities Flag [7 - 4] = 0x0 (reserved)
    // [3] = 1 ( power interlock )
    // [2] = 1 ( diagnostic interrupt )
    // [1] = 0 ( front panel lockout )
    // [0] = 0 ( intrusion sensor )
    const static unsigned char capabFlag = 0x0B;
    
    const static unsigned char fruAddress = 0x00;// unspecified
    const static unsigned char sdrAddress = 0x20;
    const static unsigned char selAddress = 0x20;
    const static unsigned char sysMgmtAddress = 0x20;
    
    // Chassis Bridge address not provided - will assume BMC address, unless set
    const static unsigned char bridgeAddress = 0x00;
    
public:
    int process( const unsigned char* request, unsigned char* response );
};

class  GetChassisStatusCMD:public cmdProcessor{
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

class  ChassisCntrlCMD:public cmdProcessor{
public:
    int process( const unsigned char* request, unsigned char* response );
};

class  ChassisResetCMD:public cmdProcessor{
public:
    int process( const unsigned char* request, unsigned char* response );
};

class  ChassisIdentifyCMD:public cmdProcessor{
public:
    int process( const unsigned char* request, unsigned char* response );
};

#endif