#ifndef BASHSCRIPTDEFINES_H
#define BASHSCRIPTDEFINES_H

namespace BashSriptDefines{
    // OPTIONAL: Specify path to bash script to be executed for each command
    // NO AREGUMENTS ARE PASSED TO THE SCRIPT (yet)
    // SCRIPT MUST RETURN 0 ON ERROR
    
    static const char* GetChassisCapabilities        = "";
    static const char* GetChassisStatus              = "";
    static const char* ChassisControl                = "";
    static const char* ChassisReset                  = "";
    static const char* ChassisIdentify               = "";
    static const char* ChassisFrontPanel             = "";
    static const char* SetChassisCapabilities        = "";
    static const char* SetPowerRestorePolicy         = "";
    static const char* SetChassisPowerCycle          = "";
    static const char* GetChassisRestartCause        = "";
    static const char* GetPOH                        = "";

    static const char* GetPEFCapabilities            = "";
    static const char* ArmPEFPostponeTimer           = "";
    static const char* SetPEFConfigurationParameters = "";
    static const char* GetPEFConfigurationParameters = "";
    static const char* SetLastProcessedEventID       = "";
    static const char* GetLastProcessedEventID       = "";
    
    static const char* GetChannelAuthentication      = "";
    static const char* GetSessionChallenge           = "";
    static const char* ActivateSession               = "";
    static const char* SetSessionPrivilegeLevel      = "";
    static const char* CloseSession                  = "";
    static const char* GetSessionInfo                = "";
    static const char* SetChannelAccess              = "";
    static const char* GetChannelAccess              = "";
    static const char* GetChannelInfo                = "";
    
    static const char* SetLANConfigurationParameters = "";
    static const char* GetLANConfigurationParameters = "";
    static const char* ActivateSOL                   = "";
    static const char* SetSOLConfigurationParameters = "";
    static const char* GetSOLConfigurationParameters = "";
}

#endif