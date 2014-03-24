#include <syslog.h>

#include <SoLCMD.h>


using namespace IpmiCommandDefines;
extern std::ofstream log_file;

GetSoLConfigCMD::GetSoLConfigCMD(GetChannelAuthCMD* getAuth)
{
    getAuthCMD = getAuth;

    unsigned char setInProgress = 0x00;
    SOLparamMap[0x00] = new ConfigParam(1, &setInProgress);

    unsigned char SoLEnable = 0x01;
    SOLparamMap[0x01] = new ConfigParam(1, &SoLEnable);
    
    unsigned char SoLAuth = 0x00;
    SOLparamMap[0x02] = new ConfigParam(1, &SoLAuth);
    
    unsigned char charAccuIntvl_SendThresh[] = {0x05, 0x64};
    SOLparamMap[0x03] = new ConfigParam(2, charAccuIntvl_SendThresh);

    unsigned char solRetry_Intvl[] = {0x03, 0xFA};
    SOLparamMap[0x04] = new ConfigParam(2, solRetry_Intvl);

    unsigned char SoLBitRate_NV = 0x07;
    SOLparamMap[0x05] = new ConfigParam(1, &SoLBitRate_NV);
    
    unsigned char SoLBitRate_V = 0x07;
    SOLparamMap[0x06] = new ConfigParam(1, &SoLBitRate_V);
    
    unsigned char SoLChannel = 0x00;
    SOLparamMap[0x07] = new ConfigParam(1, &SoLChannel, true);
    
    unsigned char SoLPort[] = {0x00, 0x00};
    SOLparamMap[0x08] = new ConfigParam(2, SoLPort, true);
}

GetSoLConfigCMD::~GetSoLConfigCMD(){
    ConfigParamMap::iterator it;
    for(it = SOLparamMap.begin(); it != SOLparamMap.end(); it++)
    {
        delete it->second;
    }
    SOLparamMap.clear();
}

int GetSoLConfigCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    syslog(LOG_NOTICE, "Processing Get SoL Configuration Parameters CMD");
    log_file << "Get SoL Configuration Parameters Command" << std::endl;
    response[0] = COMP_CODE_OK;
    
    unsigned char revMask = 0x80;
    unsigned char channelMask = 0xF;

    // The following checks that the requested channel is the currently active one
    // Remove the check if command should treat all channels as one
    if ((request[0]&channelMask)==getAuthCMD->getChannelNum()){
        log_file << "Getting parameters for this channel" << std::endl;
        if ((((request[0]&revMask) == revMask) && ((SOLparamMap[request[1]]->rev) > 0)) || (request[0]&revMask)==0){
            if  ( SOLparamMap.find(request[1]) != SOLparamMap.end() ) {
                response[1] = SOLparamMap[request[1]]->rev;
                for(int i = 0; i < (SOLparamMap[request[1]]->length); i++){
                    response[i+2] = SOLparamMap[request[1]]->data[i];
                }
                return 2+SOLparamMap[request[1]]->length;
            }
        } else {
            log_file << "Requested parameter #"<<(int)request[1]<<" revision-only requested, but none are recorded"<<std::endl;
        }        
    } else {
        response[0] = INVALID_DEVICE;
    }
    
    return 1;
}

bool GetSoLConfigCMD::setMap(unsigned char param, const unsigned char* reqData, int reqLength){
    if  ( SOLparamMap.find(param) != SOLparamMap.end() ) {
        SOLparamMap[param]->rev++;
        for(int i = 0; i < reqLength; i++){
            SOLparamMap[param]->data[i] = reqData[i+2];
        }
        return true;
    }
    return false;
}


SetSoLConfigCMD::SetSoLConfigCMD(GetSoLConfigCMD * SOLConfigCmd, GetChannelAuthCMD* getAuth):SOLConfigCMD(SOLConfigCmd), getAuthCMD(getAuth) {}

int SetSoLConfigCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    syslog(LOG_NOTICE, "Processing Set SoL Configuration Parameters CMD");
    log_file << "Set SoL Configuration Parameters Command" << std::endl;
    unsigned char channelMask = 0xF;
    
    // The following checks that the requested channel is the currently active one
    // Remove the check if command should treat all channels as one
    if ((request[0]&channelMask)==getAuthCMD->getChannelNum()){
        syslog(LOG_NOTICE, "Setting parameters for this channel CMD");
        if (SOLConfigCMD->setMap(request[1], request, (reqLength-2))) response[0] = COMP_CODE_OK;
        else response[0] = PARAM_UNSUPPORTED;
    } else {
        response[0] = INVALID_DEVICE;
    }
    
    return 1;
}

int SoLActivatingCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    syslog(LOG_NOTICE, "Processing SoL Activating CMD");
    log_file << "SoL Activating Command" << std::endl;
    response[0] = COMP_CODE_OK;
    return 1;
}
/*
GetSoLConfigCMD::GetSoLConfigCMD(): parameterRevision(0x00), setInProgress(0x00), SoLEnable(0x01), SoLAuth(0x00), charAccumulateIntervals(0x05), charSendThresh(0x64), SoLRetryCount(0x03), SoLRetryInterval(0xFA), SoLBitRate_NV(0x07), SoLBitRate_V(0x07), SoLChannel(0x00), SoLPort(0x0000) { }

int GetSoLConfigCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    log_file << "Get SoL Configuration Parameters Command" << std::endl;
    response[0] = COMP_CODE_OK;
    response[1] = parameterRevision;		//May be possible EVERY parameter has this field

    switch(request[1])
    {
	case 0:
	    response[2] = setInProgress;
	    return 3;
	case 1:
	    response[2] = SoLEnable;
	    return 3;
	case 2:
	    response[2] = SoLAuth;
	    return 3;
	case 3:
	    response[2] = charAccumulateIntervals;
	    response[3] = charSendThresh;
	    return 4;
	case 4:
	    response[2] = SoLRetryCount;
	    response[3] = SoLRetryInterval;
	    return 4;
	case 5:
	    response[2] = SoLBitRate_NV;
	    return 3;
	case 6:
	    response[2] = SoLBitRate_V;
	    return 3;
	case 7:
	    response[2] = SoLChannel;
	    return 3;
	case 8:
	    response[2] = (unsigned char)(SoLPort >> 2)&0xF;
	    response[3] = (unsigned char)SoLPort&0xF;
	    return 4;
	default:
	    return 1; //Error Should be unexpected data bytes
    }
}

unsigned char GetSoLConfigCMD::getParameterRevision()
{
    return parameterRevision;
}

unsigned char GetSoLConfigCMD::getSetInProgress()
{
    return setInProgress & 0x03;
}

unsigned char GetSoLConfigCMD::getSoLEnable()
{
    return SoLEnable&0x1;
}
unsigned char GetSoLConfigCMD::getForceEncryption()
{
    return (SoLAuth >> 7)&0x1;
}
unsigned char GetSoLConfigCMD::getForceAuthentication()
{
    return (SoLAuth >> 6)&0x1;
}
unsigned char GetSoLConfigCMD::getPriviledgeLevel()
{
    return (SoLAuth)&0xF;
}
unsigned char GetSoLConfigCMD::getSoLAuth()
{
    return SoLAuth;
}
unsigned char GetSoLConfigCMD::getCharAccumulateIntervals()
{
    return charAccumulateIntervals;
}

unsigned char GetSoLConfigCMD::getCharSendThresh()
{
    return charSendThresh;
}

unsigned char GetSoLConfigCMD::getSoLRetryCount()
{
     return SoLRetryCount&0x8;
}

unsigned char GetSoLConfigCMD::getSoLRetryInterval()
{
     return SoLRetryInterval;
}

unsigned char GetSoLConfigCMD::getSoLBitRate_NV()
{
     return SoLBitRate_NV&0xF;
}

unsigned char GetSoLConfigCMD::getSoLBitRate_V()
{
     return SoLBitRate_V&0xF;
}

unsigned char GetSoLConfigCMD::getSoLChannel()
{
     return SoLChannel;
}

unsigned short int GetSoLConfigCMD::getSoLPort()
{
     return SoLPort;
}

void GetSoLConfigCMD::setParameterRevision()
{
    parameterRevision++;
}
void GetSoLConfigCMD::setSetInProgress(unsigned char progress)
{
    setInProgress = progress;
}
void GetSoLConfigCMD::setSoLEnable(unsigned char enable)
{
    SoLEnable = enable;
}
void GetSoLConfigCMD::setSoLAuth(unsigned char auth)
{
    SoLAuth = auth;
}
void GetSoLConfigCMD::setCharAccumulateIntervals(unsigned char interval)
{
    charAccumulateIntervals = interval;
}
void GetSoLConfigCMD::setCharSendThresh(unsigned char thresh)
{
    charSendThresh = thresh;
}
void GetSoLConfigCMD::setSoLRetryCount(unsigned char retryCount)
{
    SoLRetryCount = retryCount;
}
void GetSoLConfigCMD::setSoLRetryInterval(unsigned char retryInterval)
{
    SoLRetryInterval = retryInterval;
}
void GetSoLConfigCMD::setSoLBitRate_NV(unsigned char rate)
{
    SoLBitRate_NV = rate;
}
void GetSoLConfigCMD::setSoLBitRate_V(unsigned char rate)
{
    SoLBitRate_V = rate;
}
void GetSoLConfigCMD::setSoLChannel(unsigned char channel)
{
    SoLChannel = channel;
}
void GetSoLConfigCMD::setSoLPort(unsigned char port,unsigned char port2)
{
    short int portnum = (port<<2) | port2;
    SoLPort = portnum;
}

SetSoLConfigCMD::SetSoLConfigCMD(GetSoLConfigCMD * SoLConfigCmd)
{
    solConfigCmd_ = SoLConfigCmd;
}

int SetSoLConfigCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    log_file << "Set SoL Configuration Parameters Command" << std::endl;
    solConfigCmd_->setSoLChannel(request[0]&0xF);
    switch(request[1])
    {
	case 0: 
	    solConfigCmd_->setSetInProgress(request[2]);
	    response[0] = COMP_CODE_OK;
	    break;
	case 1:
	    solConfigCmd_->setSoLEnable(request[2]);
	    response[0] = COMP_CODE_OK;
	    break;
	case 2:
	    solConfigCmd_->setSoLAuth(request[2]);
	    response[0] = COMP_CODE_OK;
	    break;
	case 3:
	    solConfigCmd_->setCharAccumulateIntervals(request[2]);
	    solConfigCmd_->setCharSendThresh(request[3]);
	    response[0] = COMP_CODE_OK;
	    break;
	case 4:
	    solConfigCmd_->setSoLRetryCount(request[2]);
	    solConfigCmd_->setSoLRetryInterval(request[3]);
	    response[0] = COMP_CODE_OK;
	    break;
	case 5:
	    solConfigCmd_->setSoLBitRate_NV(request[2]);
	    response[0] = COMP_CODE_OK;
	    break;
	case 6:
	    solConfigCmd_->setSoLBitRate_V(request[2]);
	    response[0] = COMP_CODE_OK;
	    break;
	case 7:
	    solConfigCmd_->setSoLChannel(request[2]);
	    response[0] = COMP_CODE_OK;
	    break;
	case 8:
	    solConfigCmd_->setSoLPort(request[2],request[3]);
	    response[0] = COMP_CODE_OK;
	    break;
	default:
	    response[0] = 0x80;
	    if(solConfigCmd_->getSetInProgress() != 0x00)
		response[0] = 0x81;
	//(0x80 = parameter not supported, 0x81 attempt to set inprogress when not in complete state, 0x82 attempt to write read-only parameter, 0x83 attempt to read a write-only parameter)
    }
    return 1;
}
*/

