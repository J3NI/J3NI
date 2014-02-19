#include <ChannelCMD.h>
#include <IpmiCommandDefines.h>
#include <fstream>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

int GetChannelAuthCMD::process(const unsigned char* request, unsigned char* response)
{
    log_file << "In GetChannelAuthCMD" << std::endl;
    
    response[0] = COMP_CODE_OK;
    response[1] = 0x01; //Channel Number
    response[2] = 0x01; //Authentication supported = None
    response[3] = 0x1b;
    
    return 9;
}