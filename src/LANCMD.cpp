#include <LANCMD.h>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

GetLANConfigCMD::GetLANConfigCMD()
{
    parameterRevision = 0x00;
    setInProgress = 0x00;
    AuthType = 0x01;
    AuthTypeCallback = 0x01;
    AuthTypeUser = 0x01;
    AuthTypeOperator = 0x01;
    AuthTypeAdmin = 0x01;
    AuthTypeOEM = 0x01;
    IPAddress[3] = 127;
    IPAddress[2] = 0;
    IPAddress[1] = 0;
    IPAddress[0] = 1;
    IPAddressSource = 0x01;
    MACAddress[5] = 0x00;  //Maybe memset(MACAddress, 0, 6) is a better idea....
    MACAddress[4] = 0x00;
    MACAddress[3] = 0x00;
    MACAddress[2] = 0x00;
    MACAddress[1] = 0x00;
    MACAddress[0] = 0x00;
    SubnetMask[3] = 0x00;
    SubnetMask[2] = 0x00;
    SubnetMask[1] = 0x00;
    SubnetMask[0] = 0x00;
    IPv4TTL = 0x40;
    IPv4Flags = 0x40;
    IPv4Parameter3 = 0x10;
    PrimaryRMCPPort = 0x26F;
    SecondaryRMCPPort = 0x298;
    BMC_ARPcontrol = 0x00;
    ARP_Interval = 0x00;
    GatewayIPAddress[3] = 127;
    GatewayIPAddress[2] = 0;
    GatewayIPAddress[1] = 0;
    GatewayIPAddress[0] = 1;
    GatewayMACAddress[5] = 0x00;
    GatewayMACAddress[4] = 0x00;
    GatewayMACAddress[3] = 0x00;
    GatewayMACAddress[2] = 0x00;
    GatewayMACAddress[1] = 0x00;
    GatewayMACAddress[0] = 0x00;
    BackupGatewayIPAddress[3] = 127;
    BackupGatewayIPAddress[2] = 0;
    BackupGatewayIPAddress[1] = 0;
    BackupGatewayIPAddress[0] = 1;
    BackupGatewayMACAddress[5] = 0x00;
    BackupGatewayMACAddress[4] = 0x00;
    BackupGatewayMACAddress[3] = 0x00;
    BackupGatewayMACAddress[2] = 0x00;
    BackupGatewayMACAddress[1] = 0x00;
    BackupGatewayMACAddress[0] = 0x00;
    memset(CommunityString,' ', 18);
    CommunityString[0] = 'p';
    CommunityString[1] = 'u';
    CommunityString[2] = 'b';
    CommunityString[3] = 'l';
    CommunityString[4] = 'i';
    CommunityString[5] = 'c';
    NumberOfDestinations = 0x00;
}

int GetLANConfigCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    log_file << "Get LAN Configuration Parameters Command" << std::endl;
    response[0] = COMP_CODE_OK;
    return 1;//Obviously wrong and incomplete
}

SetLANConfigCMD::SetLANConfigCMD(GetLANConfigCMD * LANConfigCmd)
{
    LANConfigCmd_ = LANConfigCmd;
}

int SetLANConfigCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    log_file << "Set LAN Configuration Parameters Command" << std::endl;
    response[0] = COMP_CODE_OK;
    return 1;//Obviously wrong and incomplete
}
