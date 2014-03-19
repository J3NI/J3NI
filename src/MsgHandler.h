#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include <map>
#include <stddef.h>

#include <IpmiMessage.h>
#include <IpmiCommandDefines.h>
#include <BashScriptDefines.h>

#include <ChassisCMD.h>
#include <ChannelCMD.h>
#include <LANCMD.h>
#include <PefCMD.h>
#include <SessionCMD.h>
#include <SoLCMD.h>

#include <fstream>
#include <stdlib.h>

class I_Command;
class IpmiMessage;

class MsgHandler {
    
private:
    typedef std::map<unsigned char, I_Command*> CommandMap;
    typedef std::map<int, const char*>  bashMap;
    static bashMap bashMap_;
    static CommandMap AppCommands_;
    static CommandMap ChassisCommands_;
    static CommandMap SECommands_;
    static CommandMap StorageCommands_;
    static CommandMap FirmCommands_;
    static CommandMap TransportCommands_;
    static CommandMap BridgeCommands_;
    
public:
    static bool BashOK(unsigned char netFn, unsigned char  cmd);
    
    static void initBash();
    static void initCMD(const char* user=NULL);
    static void clearCMD();
    
    static bool isPing(const IpmiMessage& message);
    static void pong(const IpmiMessage& message, IpmiMessage& response);
    static void processRequest(const IpmiMessage& message, IpmiMessage& response);
 };

#endif
