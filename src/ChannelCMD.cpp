#include <ChannelCMD.h>
#include <IpmiCommandDefines.h>
#include <fstream>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

int GetChannelAuthCMD::process(const unsigned char* request, int reqLength, unsigned char* response)
{
    log_file << "In GetChannelAuthCMD" << std::endl;
    
    response[0] = COMP_CODE_OK;
    response[1] = channelNum; //Channel Number
    response[2] = 0x01; //Authentication supported = None
    response[3] = 0x1f; // 0001 1111
			// [7:6]  00 	reserved
			// [5]	  0 	KG status (applies to v2.0/RMCP+ RAKP Auth only). Otherwise, 					    	reserved
			// [4]	  1	Per-message authentication is disabled - CHECK?
			// [3]	  1	User level authentication is disabled - CHECK?
			// [2]	  1	Non-null usernames enabled
			// [1]	  1	Null usernames enabled
			// [0]	  1	Anonymous login enabled
    response[4] = 0x00;	// reserved for IPMI v1.5
    response[5] = 0x00; // OEM IP not available, [5:7] = 0
    response[6] = 0x00;
    response[7] = 0x00;
    response[8] = 0x00;	// OEM auth type not available
    
    return 9;
}

unsigned char GetChannelAuthCMD::getChannelNum(){
    return channelNum;
}
/*
int GetChannelCipherSuites::process(const unsigned char* request, int reqLength, unsigned char* response)
{
   response[0] = COMP_CODE_OK;	// Completion code
   if(request[0]==0x0E) 	// return channel num for channel cmd was received on
	response[1] = channelNum;
   else			// else return channel num received
   	response[1] = request[0];

   response[3] = 0xC0;	// start of record byte (for standard Cipher Suite ID)
   response[4] = 0x00;  // *** CHECK **** Cipher Suite ID
   response[5] = 0x00;	// Tag = 00b, Authentication Algorithm number - RAKP-none
   response[6] = 0x40;  // Tag = 01b, Integrity Alg number - none
   response[7] = 0x80;	// Tag = 10b, Confidentiality Alg number - none

   return 18;
}

GetChannelAccessCMD::GetChannelAccessCMD(SetChannelAccessCMD* cPrivLvl): chanAccCMD(cPrivLvl);

int GetChannelAccessCMD::process(const unsigned char* request, int reqLength, unsigned char* response)
{
   response[0] = COMP_CODE_OK;
   response[1] = 0x3A;	// 0011 1010
			// [7:6] 00b	reserved
			// [5]	 1	alerting disabled
			// [4]	 1	per-message auth disabled
			// [3]	 1	user-level auth disabled
			// [2:0] 0x2	access mode is always available
   response[2] = cPrivLvl;	// 0000 cPrivLvl
			// [7:4] 0000	reserved
			// [3:0] 	Channel Privilege Level Limit
   return 3;
}

int GetChannelInfoCMD::process(const unsigned char* request, int reqLength, unsigned char* response)
{
   response[0] = COMP_CODE_OK;	// Completion code
   if(request[0]==0x0E) 	// return channel num for channel cmd was received on
	response[1] = channelNum;
   else			// else return channel num received
   	response[1] = request[0];  
   // Our implementation only uses one channel so no need to check other channels
   response[2] = 0x04	// 
			// Channel Medium Type **********
   response[3] = 0x01	//
			// Channel Protocol Type ********
   unsigned char sesSupportByte;
   response[4] = 0x81	//
			// Session support **************
			// Num of sessions activated
   response[5:7] = 0xF21B00 //IPMI Enterprise Number
   response[9:10] = 0xFFFF // Auxiliary Chan Info *************
   return 10; 
}

int SetChannelAccessCMD::process(const unsigned char* request, int reqLength, unsigned char* response)
{
   unsigned char newLvl = request[2] & 0x0F;
   if(SetChanPrivLvl(newLvl)==1 && (request[2]&0xC0)!=0x00)
	response[0] = COMP_CODE_OK;
   else
	response[0] = 0x82; // sets not supported on selected channel
   if(request[1]&0xC0!=0x00)
   {
	// 1	PEF Alerting always disabled
	// 1	Per message Authentication always disabled
	// 1	User lvl authentication always disabled
	// 010  Access Mode for IPMI messaging - assumed always available
   }
   return 1; 
}

int SetChannelAccessCMD::SetChanPrivLvl( const unsigned char newLvl )
{
    if ((newLvl >= 1) && (newLvl <= 4)) {
        chanPrivLvl = newLvl;
        return 1;
    }
    return 0; 
}*/
