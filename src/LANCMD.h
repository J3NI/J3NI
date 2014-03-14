#ifndef LANCMD_H
#define LANCMD_H

#include <fstream>
#include <string.h>

#include <ConfigParam.h>
#include <I_Command.h>
#include <MsgHandler.h>
#include <IpmiCommandDefines.h>

class GetChannelAuthCMD;

class GetLANConfigCMD:public I_Command
{
private:
    GetChannelAuthCMD* getAuthCMD;

    ConfigParamMap LANparamMap;
    
    //Configuration Parameter Data
    //Set in Progress (Volatile)
    //[7-2] Reserved
    //[1-0] = 00 (00 Complete, 01 In Progress, 10 OPTIONAL commit write, 11 Reserved)
    //Default = 0x00
    
    //Authentication Type (Read-Only)
    //[7-6] Reserved
    //[5] = 0 OEM Propriety
    //[4] = 0 Straight Password/Key
    //[3] Reserved
    //[2] = 0 MD5
    //[1] = 0 MD2
    //[0] = 1 NONE
    //Default = 0x01
    
    //Authentication Type Enables
    //[7-6] Reserved
    //[5] = 0 OEM Propriety
    //[4] = 1 Straight Password/Key
    //[3] Reserved
    //[2] = 0 MD5
    //[1] = 0 MD2
    //[0] = 1 NONE
    
    //IP Address
    //Default = 127.0.0.1
    
    //IP Address Source
    //[7-4] Reserved
    //[3-0] = 0x01 Address Source (0x00 Unspecified, 0x01 Static Address manually configured)
    //Default = 0x01
    
    //MAC Address
    //Default = 0x000000
    
    //Subnet Mask
    //Default = 0x0000, maybe 0xFFF0 later?
    
    //IPv4 Header Parameters
    // byte 2: [7-0] Time to live
    // byte 1: [7-5] = 010b Flags
    // byte 1: [4-0] Reserved
    // byte 0: [7-5] = 000b Precedence
    // byte 0: [4-1] = 1000b Type of Service
    // byte 0: [0] Reserved
    
    //Primary RMCP Port Number (OPTIONAL)
    //Default = 0x26F
    
    //Secondary RMCP Port Number (OPTIONAL)
    //Default = 0x298
    
    //BMC-generated ARP control (OPTIONAL)
    //[7-2] Reserved
    //[1] = 0
    //[0] = 0
    //Default = 0x00
    
    //Gratuitous ARP Interval (OPTIONAL)
    //Default = 0x00
    
    //Default Gateway Address
    //Default = 127.0.0.1
    
    //Default Gateway MAC Address
    //Default = 0x000000
    
    //Backup Gateway Address
    //Default = 127.0.0.1
    
    //Backup Gateway MAC Address
    //Default = 0x000000
    
    //Community String
    //Default "J3NIIPMI"
    
    //Number of Destinations (Read-Only)
    //[7-4] Reserved
    //[3-0] = 0x00 Number LAN Destinations (0x00 indicates LAN Alerting not supported)
    
    //Destination Type DOESN'T EXIST since Number of Destinations set to 0x00
    //Destination Addresses DOESN'T EXIST since Number of Destinations set to 0x00

public:
    GetLANConfigCMD(GetChannelAuthCMD* getAuth);
    ~GetLANConfigCMD();
    bool setMap(unsigned char param, const unsigned char* reqData, int reqLength);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class SetLANConfigCMD:public I_Command
{
private:
    GetLANConfigCMD * LANConfigCMD;
    GetChannelAuthCMD* getAuthCMD;
    
public:
    SetLANConfigCMD(GetLANConfigCMD * LANConfigCmd, GetChannelAuthCMD* getAut);
    int process( const unsigned char* request, int reqLength, unsigned char* response );
};


#endif
