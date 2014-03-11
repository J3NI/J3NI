#include <MsgHandler.h>
#include <SoLCMD.h>
#include <IpmiCommandDefines.h>
#include <fstream>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

int SoLActivatingCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    log_file << "SoL Activating Command" << std::endl;
    response[0] = COMP_CODE_OK;
    return 1;
}

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


