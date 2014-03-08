#include <MsgHandler.h>
#include <SoLCMD.h>
#include <IpmiCommandDefines.h>
#include <fstream>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

int  SoLActivatingCMD::process( const unsigned char* request, int reqLength, unsigned char* response )
{
    response[0] = COMP_CODE_OK;
    return 1;
}


