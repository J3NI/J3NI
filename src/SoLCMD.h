#ifndef SOLCMD_H
#define SOLCMD_H

#include <I_Command.h>

class  SoLActivatingCMD:public I_Command{
public:
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class GetSoLConfigCMD:public I_Command{
private:
    //Parameter Revision NOTE: Most values are 0 before the first SET, however some default values are given based on intel/IBM setup guides
    //[7-0] = 0x00 to begin with
    unsigned char parameterRevision;  //Default = 0x00

    //Configuration Parameter Data
    //Set in Progress (Volatile)
    //[7-2] Reserved
    //[1-0] = 00 (00 Complete, 01 In Progress, 10 OPTIONAL commit write, 11 Reserved)
    unsigned char setInProgress; //Default = 0x00
    
    //SoL Enable
    //[7-1] Reserved
    //[0] = 1 SOL Enable (1 = enable SOL payload, 0 = disable payload) 1 lets SOL activate to work
    unsigned char SoLEnable; //Default = 0x01

    //SoL Authentication
    //[7] = 0 Force SOL Payload Encryption (0 = encryption dependent on remote console, 1 = if supported always encryption)
    //[6] = 0 Force SOL Payload Authentication (0 = authentication controlled by remote software if encryption is being used authentication must be used, 1 = if support always authenticate SOL payload data)
    //[5-4] Reserved
    //[3-0] = 0x00 SOL Priviledge Level (0x2 User, 0x3 Operator, 0x4 Admin, 0x5 OEM, all others reserved)
    unsigned char SoLAuth; //Default = 0x00

    //Character Accumulate Interval & Character Send Threshold
    //[7-0] = 5 Character Accumulate Interval (1-based 5ms increments)
    //[7-0] = 100 Character Send Threshold(1-based sends data packet when this number of characters has been accepted) maybe default value could be more...
    unsigned char charAccumulateIntervals; //Default = 0x05
    unsigned char charSendThresh; //Default = 0x64

    //SoL Retry
    //[7-3] Reserved
    //[2-0] = 3 Retry Count (1-based 0 = no retries after packet is transmitted, dropped if no ACK/NACK)
    //[7-0] = 250 Retry Interval (1-based retry interval in 10ms increments)
    unsigned char SoLRetryCount; //Default = 0x03
    unsigned char SoLRetryInterval; //Default = 0xFA

    //SoL non-volatile bit rate (non-volatile)
    //NOT SUPPORTED IF NO BMC SERIAL CONTROLLER THAN CAN BE POTENTIALLY CONFIGURED
    //[7-4] Reserved
    //[3-0] = 0x07 Bit Rate (0x00 Use present configuration, 0x6 9600 bps, 0x7 19.2 kbps, 0x8 38.4 kbps, 0x9 57.6 kpbs, 0xA 115.2 kpbs, all others reserved)
    unsigned char SoLBitRate_NV; //Default = 0x07

    //SoL volatile bit rate
    //[7-4] Reserved
    //[3-0] = 0x07
    unsigned char SoLBitRate_V; //Default = 0x07

	//These 2 don't appear on the IPMI SoL Set Parameters when using IPMItool
    //SoL Payload Channel (OPTIONAL)
    unsigned char SoLChannel; //Default = 0x00
    //SoL Payload Port Number (Read-only or Read/Write)
    unsigned short int SoLPort; //Default = 0x0000
    
    public:
    GetSoLConfigCMD();
    unsigned char getParameterRevision();
    unsigned char getSetInProgress();
    unsigned char getSoLEnable();
    unsigned char getForceEncryption();
    unsigned char getForceAuthentication();
    unsigned char getPriviledgeLevel();
    unsigned char getSoLAuth();
    unsigned char getCharAccumulateIntervals();
    unsigned char getCharSendThresh();
    unsigned char getSoLRetryCount();
    unsigned char getSoLRetryInterval();
    unsigned char getSoLBitRate_NV();
    unsigned char getSoLBitRate_V();
    unsigned char getSoLChannel();
    unsigned short int getSoLPort();
    void setParameterRevision();
    void setSetInProgress(unsigned char progress);
    void setSoLEnable(unsigned char enable);
    void setSoLAuth(unsigned char auth);
    void setCharAccumulateIntervals(unsigned char interval);
    void setCharSendThresh(unsigned char thresh);
    void setSoLRetryCount(unsigned char retryCount);
    void setSoLRetryInterval(unsigned char retryInterval);
    void setSoLBitRate_NV(unsigned char rate); 
    void setSoLBitRate_V(unsigned char rate);
    void setSoLChannel(unsigned char channel);
    void setSoLPort(unsigned short int port);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class SetSoLConfigCMD:public I_Command{
    private:
    GetSoLConfigCMD * solConfigCmd_;
    public:
    SetSoLConfigCMD(GetSoLConfigCMD * SoLConfigCmd);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

#endif
