#include <LANCMD.h>
#include <ChannelCMD.h>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

GetLANConfigCMD::GetLANConfigCMD(GetChannelAuthCMD* getAuth)
{
    getAuthCMD = getAuth;

    LANparamMap[0x00] = new LanParam(1, new unsigned char(0x00));
    LANparamMap[0x01] = new LanParam(1, new unsigned char(0x01));
    
    unsigned char AuthTypeEnableData[] = {0x11, 0x11, 0x11, 0x11, 0x11} ;
    LANparamMap[0x02] = new LanParam(5, AuthTypeEnableData);
    
    unsigned char IPAddressData[] = {0x7f, 0x00, 0x00, 0x01};
    LANparamMap[0x03] = new LanParam(4, IPAddressData);

    LANparamMap[0x04] = new LanParam(1, new unsigned char(0x01));

    LANparamMap[0x05] = new LanParam(1, new unsigned char[5]());

    LANparamMap[0x06] = new LanParam(1, new unsigned char[3]());
    
    unsigned char IPv4HeaderData[] = {0x40,0x40,0x10} ;
    LANparamMap[0x07] = new LanParam(3, IPv4HeaderData);
    
    unsigned char PrimaryRMCPPortData[] = {0x02, 0x6F};
    LANparamMap[0x08] = new LanParam(2, PrimaryRMCPPortData);
    
    unsigned char SecondaryRMCPPortData[] = {0x02, 0x98};
    LANparamMap[0x09] = new LanParam(2, SecondaryRMCPPortData);
    
    LANparamMap[0x0A] = new LanParam(1, new unsigned char(0x00));

    LANparamMap[0x0B] = new LanParam(1, new unsigned char(0x00));
    
    LANparamMap[0x0C] = new LanParam(4, IPAddressData);

    LANparamMap[0x0D] = new LanParam(5, new unsigned char[5]());

    LANparamMap[0x0E] = new LanParam(4, IPAddressData);
    
    LANparamMap[0x0F] = new LanParam(5, new unsigned char[5]());
    
    unsigned char communityStringData[] = {'J','3','N','I','I','P','M','I',0,0,0,0,0,0,0,0,0,0};
    LANparamMap[0x10] = new LanParam(18,  communityStringData);
    
    LANparamMap[0x11] = new LanParam(1, new unsigned char(0x00));
}


int GetLANConfigCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    log_file << "Get LAN Configuration Parameters Command" << std::endl;
    response[0] = COMP_CODE_OK;
    
    unsigned char revMask = 0x80;
    
    if (request[0]&(getAuthCMD->getChannelNum())==getAuthCMD->getChannelNum()){
        log_file << "Getting parameters for this channel" << std::endl;
        if ((((request[0]&revMask) == revMask) && ((LANparamMap[request[1]]->rev) > 0)) || (request[0]&revMask)==0){
            if  ( LANparamMap.find(request[1]) != LANparamMap.end() ) {
                response[1] = LANparamMap[request[1]]->rev;
                for(int i = 0; i < (LANparamMap[request[1]]->length); i++){
                    response[i+2] = LANparamMap[request[1]]->data[i];
                }
                return 2+LANparamMap[request[1]]->length;
            }
        } else {
            log_file << "Requested parameter #"<<(int)request[1]<<" revision-only requested, but none are recorded"<<std::endl;
        }        
    }
    
    return 1;
}

bool GetLANConfigCMD::setMap(unsigned char param, const unsigned char* reqData, int reqLength){
    if  ( LANparamMap.find(param) != LANparamMap.end() ) {
        LANparamMap[param]->rev++;
        for(int i = 0; i < reqLength; i++){
            LANparamMap[param]->data[i] = reqData[i+2];
        }
        return true;
    }
    return false;
}


SetLANConfigCMD::SetLANConfigCMD(GetLANConfigCMD * LANConfigCmd, GetChannelAuthCMD* getAuth):LANConfigCMD(LANConfigCmd), getAuthCMD(getAuth) {}

int SetLANConfigCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    log_file << "Set LAN Configuration Parameters Command" << std::endl;    
    
    if (request[0]&(getAuthCMD->getChannelNum())==getAuthCMD->getChannelNum()){
        log_file << "Setting parameters for this channel" << std::endl;
        if (LANConfigCMD->setMap(request[1], request, (reqLength-2))) response[0] = COMP_CODE_OK;
        else response[0] = PARAM_UNSUPPORTED;
    }
    
    return 1;
}
