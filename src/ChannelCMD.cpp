#include <fstream>
#include <syslog.h>

#include <ChannelCMD.h>
#include <IpmiCommandDefines.h>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;


GetChannelAuthCMD::GetChannelAuthCMD():channelNum(CHANNEL_NUMBER){}

int GetChannelAuthCMD::process(const unsigned char* request, int reqLength, unsigned char* response)
{
    syslog(LOG_NOTICE, "Processing Get Channel Authentication CMD");
    log_file << "In GetChannelAuthCMD" << std::endl;
    
    response[0] = COMP_CODE_OK;
    response[1] = channelNum; //Channel Number
    response[2] = 0x11; //Authentication supported 
    response[3] = 0x1f; // 0001 1111
    // [7:6]  00 	reserved
    // [5]	  0 	KG status (applies to v2.0/RMCP+ RAKP Auth only). Otherwise - reserved
    // [4]	  1	Per-message authentication is disabled
    // [3]	  1	User level authentication is disabled
    // [2]	  1	Non-null usernames enabled
    // [1]	  1	Null usernames enabled
    // [0]	  1	Anonymous login enabled
    response[4] = 0x00;	// reserved for IPMI v1.5
    response[5] = 0x00;  // OEM IP not available, [5:7] = 0
    response[6] = 0x00;
    response[7] = 0x00;
    response[8] = 0x00;	// OEM auth type not available
    
    return 9;
}

unsigned char GetChannelAuthCMD::getChannelNum(){
    return channelNum;
}

GetChannelInfoCMD::GetChannelInfoCMD(GetChannelAuthCMD* getAuth): getAuthCMD(getAuth) {}
int GetChannelInfoCMD::process(const unsigned char* request, int reqLength, unsigned char* response)
{
    syslog(LOG_NOTICE, "Processing Get Channel Info CMD");
    
    log_file << "Processing Get Channel Info CMD" << std::endl;
    response[0] = COMP_CODE_OK;	// Completion code
    if(request[0]==0x0E) 	// return channel num for channel cmd was received on
        response[1] = getAuthCMD->getChannelNum();
    else			// else return channel num received
        response[1] = request[0];
    // Our implementation only uses one channel so no need to check other channels
    response[2] = 0x04;	// Channel Medium Type: 802.3 LAN
    response[3] = 0x01;	// Channel Protocol Type: IPMB-1.0, serial/modem Basic Mode, and LAN
    response[4] = 0x81;	// Session support: multi-session, # sessions activated on channel = 1
    response[5] = 0xF2; // IPMI Enterprise Number = 0xF21B00
    response[6] = 0x1B;
    response[7] = 0x00;
    response[8] = 0xFF; // Auxiliary Chan Info = 0xFFFF (no interrupt/unspecified)
    response[9] = 0xFF;
    return 10;
}


SetChannelAccessCMD::SetChannelAccessCMD(): accessSettings(0x3f), privLvl(0x04) {}

int SetChannelAccessCMD::process(const unsigned char* request, int reqLength, unsigned char* response)
{
    syslog(LOG_NOTICE, "Processing Set Channel Access CMD");
    log_file << "Processing Set Channel Access CMD" << std::endl;
    unsigned char setMask = 0xC0;
    unsigned char accMask = 0x1F;
    unsigned char lvlMask = 0x07;
    
    if ( (request[1]&setMask) > 0){
        // Setting Channel Access
        accessSettings = accessSettings&(request[1]&accMask);
    }
    
    if ( (request[2]&setMask) > 0){
        // Setting Channel Privilege level
        if ( (request[2]&0x07) == 0x00) response[0] = ACC_MODE_UNSUPPORTED;
        else privLvl = request[2]&lvlMask;
    }
    return 1;
}


unsigned char SetChannelAccessCMD::getPrivLvl()
{
    return privLvl;
}

unsigned char SetChannelAccessCMD::getSettings()
{
    return accessSettings;
}

GetChannelAccessCMD::GetChannelAccessCMD(SetChannelAccessCMD* chanAcc): chanAccCMD(chanAcc) {}

int GetChannelAccessCMD::process(const unsigned char* request, int reqLength, unsigned char* response)
{
    syslog(LOG_NOTICE, "Processing Get Channel Access CMD");
    log_file << "Processing Get Channel Access CMD" << std::endl;
    response[0] = COMP_CODE_OK;
    response[1] = chanAccCMD->getSettings();
    response[2] = chanAccCMD->getPrivLvl();
    return 3;
}
