#ifndef LANCMD_H
#define LANCMD_H

#include <I_Command.h>
#include <string.h>
#include <MsgHandler.h>
#include <IpmiCommandDefines.h>
#include <fstream>

class GetLANConfigCMD:public I_Command{
private:
    //Parameter Revision
    //[7-0] = 0x00 to begin with
    unsigned char parameterRevision;  //Default = 0x00

    //Configuration Parameter Data
    //Set in Progress (Volatile)
    //[7-2] Reserved
    //[1-0] = 00 (00 Complete, 01 In Progress, 10 OPTIONAL commit write, 11 Reserved)
    unsigned char setInProgress; //Default = 0x00

    //Authentication Type (Read-Only)
    //[7-6] Reserved
    //[5] = 0 OEM Propriety
    //[4] = 0 Straight Password/Key
    //[3] Reserved
    //[2] = 0 MD5
    //[1] = 0 MD2
    //[0] = 1 NONE
    unsigned char AuthType; //Default = 0x01

    //Authentication Type Enables
    //Byte 1
    //[7-6] Reserved
    //[5] = 0 OEM Propriety
    //[4] = 0 Straight Password/Key
    //[3] Reserved
    //[2] = 0 MD5
    //[1] = 0 MD2
    //[0] = 1 NONE
    unsigned char AuthTypeCallback; //Default = 0x01
    //Byte 2
    unsigned char AuthTypeUser; //Default = 0x01
    //Byte 3
    unsigned char AuthTypeOperator; //Default = 0x01
    //Byte 4
    unsigned char AuthTypeAdmin; //Default = 0x01
    //Byte 5
    unsigned char AuthTypeOEM; //Default = 0x01

    //IP Address
    unsigned char IPAddress[4]; //Default = 127.0.0.1

    //IP Address Source
    //[7-4] Reserved
    //[3-0] = 0x01 Address Source (0x00 Unspecified, 0x01 Static Address manually configured)
    unsigned char IPAddressSource;  //Default = 0x01

    //MAC Address
    unsigned char MACAddress[6]; //Default = 0x000000

    //Subnet Mask
    unsigned char SubnetMask[4]; //Default = 0x0000, maybe 0xFFF0 later?

    //IPv4 Header Parameters
    //[7-0] Time to live
    unsigned char IPv4TTL; //Default = 0x40
    //[7-5] = 010b Flags
    //[4-0] Reserved
    unsigned char IPv4Flags; //Default = 0x40
    //[7-5] = 000b Precedence
    //[4-1] = 1000b Type of Service
    //[0] Reserved
    unsigned char IPv4Parameter3; //Default = 0x10

    //Primary RMCP Port Number (OPTIONAL)
    unsigned short int PrimaryRMCPPort; //Default = 0x26F

    //Secondary RMCP Port Number (OPTIONAL)
    unsigned short int SecondaryRMCPPort; //Default = 0x298

    //BMC-generated ARP control (OPTIONAL)
    //[7-2] Reserved
    //[1] = 0 
    //[0] = 0 
    unsigned char BMC_ARPcontrol; //Default = 0x00

    //Gratuitous ARP Interval (OPTIONAL)
    unsigned ARP_Interval; //Default = 0x00

    //Default Gateway Address
    unsigned char GatewayIPAddress[4]; //Default = 127.0.0.1

    //Default Gateway MAC Address
    unsigned char GatewayMACAddress[6]; //Default = 0x000000

    //Backup Gateway Address 
    unsigned char BackupGatewayIPAddress[4]; //Default = 127.0.0.1

    //Backup Gateway MAC Address
    unsigned char BackupGatewayMACAddress[6]; //Default = 0x000000

    //Community String
    unsigned char CommunityString[18]; //Default = 'public'

    //Number of Destinations (Read-Only)
    //[7-4] Reserved
    //[3-0] = 0x00 Number LAN Destinations (0x00 indicates LAN Alerting not supported)
    unsigned char NumberOfDestinations; //Default = 0x00

    //Destination Type DOESN'T EXIST since Number of Destinations set to 0x00

    //Destination Addresses DOESN'T EXIST since Number of Destinations set to 0x00

public:
GetLANConfigCMD();
int process( const unsigned char* request, int reqLength, unsigned char* response );
};

class SetLANConfigCMD:public I_Command{
private:
GetLANConfigCMD * LANConfigCmd_;
public:
SetLANConfigCMD(GetLANConfigCMD * LANConfigCmd);
int process( const unsigned char* request, int reqLength, unsigned char* response );
};


#endif
