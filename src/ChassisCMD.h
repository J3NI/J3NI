#ifndef CHASSISCMD_H
#define CHASSISCMD_H

#include <I_Command.h>

class  GetChassisCapabCMD:public I_Command{
private:
    // Capabilities Flag - Read Only
    // [7 - 4] = 0x0 (reserved)
    // [3] = 1 ( power interlock )
    // [2] = 1 ( diagnostic interrupt )
    // [1] = 1 ( front panel lockout )
    // [0] = 0 ( intrusion sensor )
    const static unsigned char capabFlag = 0x0E;
    
    // Aill assume BMC address by default, can set with Set Chassis Capabilities Command
    unsigned char fruAddress;
    unsigned char sdrAddress;
    unsigned char selAddress;
    unsigned char sysMgmtAddress;
    
    // Chassis Bridge address not provided, unless set
    unsigned char bridgeAddress;
    
public:
    GetChassisCapabCMD();
    unsigned char getCapabFlag();
    unsigned char getFruAddress();
    unsigned char getSdrAddress();
    unsigned char getSelAddress();
    unsigned char getSysMgmtAddress();
    unsigned char getBridgeAddress();
    void setAllFields( const unsigned char* data, int bridgeSet);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
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
    unsigned char miscChassisState;
    
    // Front Panel Button Capabilities not supported
    
    
public:
    GetChassisStatusCMD();
    unsigned char getCurPowerState();
    unsigned char getLastPowerEvent();
    unsigned char getMiscChassisState();
    void setPowerState(int powerState);
    void setPowerPolicy(unsigned char policy);
    unsigned char getPowerPolicy();
    void setLastPowerEvent(unsigned char eventByte);
    void setMiscChassisState(unsigned char chassisState);
    int getCurrentPower();
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class  ChassisResetCMD:public I_Command{
public:
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class  ChassisIdentifyCMD:public I_Command{
public:
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class  ChassisFrontPanelCMD:public I_Command{
private:
    GetChassisStatusCMD* statusCmd_;
public:
    ChassisFrontPanelCMD(GetChassisStatusCMD* chassisStatusCmd);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class  SetChassisCapabCMD:public I_Command{
private:
    GetChassisCapabCMD* capabCmd_;
public:
    SetChassisCapabCMD(GetChassisCapabCMD* chassisCapabCmd);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class  SetChassisPowerRestore:public I_Command{
private:
    GetChassisStatusCMD* statusCmd_;
public:
    SetChassisPowerRestore(GetChassisStatusCMD* chassisStatusCmd);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class  SetChassisPowerCycle:public I_Command{
public:
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class  GetChassisRestartCause:public I_Command{
private:
    unsigned char restartCause;
public:
    GetChassisRestartCause();
    unsigned char getRestartCause();
    void setRestartCause(unsigned char cause);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class  ChassisCntrlCMD:public I_Command{
private:
    GetChassisStatusCMD* statusCmd_;
    GetChassisRestartCause* restartCause_;
    
public:
    ChassisCntrlCMD(GetChassisStatusCMD* chassisStatusCmd, GetChassisRestartCause* restartCause);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class  GetChassisPOHCounter:public I_Command{
private:
    time_t startTime;
public:
    GetChassisPOHCounter();
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};
#endif
