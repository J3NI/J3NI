#ifndef BASHSCRIPTDEFINES_H
#define BASHSCRIPTDEFINES_H

namespace bashSripts{
    // OPTIONAL: Specify path to bash script to be executed for each command
    // NO AREGUMENTS ARE PASSED TO THE SCRIPT (yet)
    // SCRIPT MUST RETURN 0 ON ERROR
    
    const char* GetChassisCapabilities        = "";
    const char* GetChassisStatus              = "";
    const char* ChassisControl                = "";
    const char* ChassisReset                  = "";
    const char* ChassisIdentify               = "";
    const char* ChassisFrontPanel             = "";
    const char* SetChassisCapabilities        = "";
    const char* SetPowerRestorePolicy         = "";
    const char* SetChassisPowerCycle          = "";
    const char* GetChassisRestartCause        = "";
    const char* GetPOH                        = "";

    const char* GetPEFCapabilities            = "";
    const char* ArmPEFPostponeTimer           = "";
    const char* SetPEFConfigurationParameters = "";
    const char* GetPEFConfigurationParameters = "";
    const char* SetLastProcessedEventID       = "";
    const char* GetLastProcessedEventID       = "";
    
    const char* GetChannelAuthentication      = "";
    const char* GetSessionChallenge           = "";
    const char* ActivateSession               = "";
    const char* SetSessionPrivilegeLevel      = "";
    const char* CloseSession                  = "";
    const char* GetSessionInfo                = "";
    const char* SetChannelAccess              = "";
    const char* GetChannelAccess              = "";
    const char* GetChannelInfo                = "";
    
    const char* SetLANConfigurationParameters = "";
    const char* GetLANConfigurationParameters = "";
    const char* ActivateSOL                   = "";
    const char* SetSOLConfigurationParameters = "";
    const char* GetSOLConfigurationParameters = "";
};

#endif