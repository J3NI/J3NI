#ifndef SOLCMD_H
#define SOLCMD_H

#include <I_Command.h>

class  SoLActivatingCMD:public I_Command{
public:
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class GetSoLConfigCMD:public I_Command{
private:
    //Parameter Revision
    //[7-0] NOT SURE what to put for DEFAULT VALUE
    unsigned char parameterRevision;

    //Configuration Parameter Data
    //Set in Progress (Volatile)
    //[7-2] Reserved
    //[1-0] = 0 (00 Complete, 01 In Progress, 10 OPTIONAL commit write, 11 Reserved)
    unsigned char setInProgress;
    
    //SoL Enable
    //[7-1] Reserved
    //[0] = 1 SOL Enable (1 = enable SOL payload, 0 = disable payload) 1 lets SOL activate to work
    unsigned char SoLEnable;

    //SoL Authentication
    //[7] = 0 Force SOL Payload Encryption (0 = encryption dependent on remote console, 1 = if supported always encryption)
    //[6] = 0 Force SOL Payload Authentication (0 = authentication controlled by remote software if encryption is being used authentication mut be used, 1 = if support always authenticate SOL payload data)
    //[5-4] Reserved
    //[3-0] = 0x04 SOL Priviledge Level (0x2 User, 0x3 Operator, 0x4 Admin, 0x5 OEM, all others reserved)
    unsigned char SoLAuth;

    //Character Accumulate Interval & Character Send Threshold
    //[7-0] = 0x1 Character Accumulate Interval (1-based 5ms increments)
    //[7-0] = 1 Character Send Threshold(1-based sends data packet when this number of characters has been accepted) maybe default value could be more...
    unsigned char charAccumulateIntervals;
    unsigned char charSendThresh;

    //SoL Retry
    //[7-3] Reserved
    //[2-0] = 0 Retry Count (1-based 0 = no retries after packet is transmitted, dropped if no ACK/NACK)
    //[7-0] = 0x00 Retry Interval (1-based retry interval in 10ms increments)
    unsigned char SoLRetryCount;
    unsigned char SoLRetryInterval;

    //SoL non-volatile bit rate (non-volatile)
    //NOT SUPPORTED IF NO BMC SErIAL CONTROLLER THAN CAN BE POTENTIALLY CONFIGURED
    //[7-4] Reserved
    //[3-0] = 0x00 Bit Rate (0x00 Use present configuration, 0x6 9600 bps, 0x7 19.2 kbps, 0x8 38.4 kbps, 0x9 57.6 kpbs, 0xA 115.2 kpbs, all others reserved)
    unsigned char SoLBitRate_NV;

    //SoL volatile bit rate
    //[7-0] = ??
    unsigned char SoLBitRate_V;

    //SoL Payload Channel (OPTIONAL)

    //SoL Payload Port Number (Read-only or Read/Write)
    
};

#endif
