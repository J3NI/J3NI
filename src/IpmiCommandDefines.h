//
//  IpmiCommandDefines.h
//  J3NI
//
//  Created by Neil on 2014-01-29.
//  Copyright (c) 2014 Neil. All rights reserved.
//

#ifndef IPMICOMMANDDEFINES_H
#define IPMICOMMANDDEFINES_H

namespace IpmiCommandDefines
{
    const unsigned int PING_LENGTH = 12;    
    const unsigned int PONG_LENGTH = 27;
    
    const unsigned char PING_REQUEST[PING_LENGTH] = {
        0x06,0x00,0xff,0x06,0x00,0x00,0x11,0xbe,0x80,0x00,0x00,0x00};
    const unsigned char PONG_RESPONSE[PONG_LENGTH] = {
        0x06,0x00,0xff,0x06,0x00,0x00,0x11,0xbe,0x40,0x00,0x10,0x00,
        0x00,0x11,0xbe,0x00,0x00,0x00,0x00,0x81,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00};
    
    
    const unsigned int LUN_MASK                 = 0x03;
    const unsigned int NET_FN_MASK              = 0xFC;
    const unsigned int SEQUENCE_MASK            = 0xFC;
    
    const unsigned int RMCP_HEADER_INDEX        = 0;
    const unsigned int AUTH_TYPE_INDEX          = 4;
    const unsigned int SESSION_NUM_INDEX        = 5;
    const unsigned int SESSION_ID_INDEX         = 9;
    const unsigned int LENGTH_INDEX             = 13;
    const unsigned int DEST_ADDRESS_INDEX       = 14;
    const unsigned int NET_FN_INDEX             = 15;
    const unsigned int CHECKSUM_INDEX           = 16;
    const unsigned int SOURCE_ADDRESS_INDEX     = 17;
    const unsigned int REQUEST_SEQUENCE_INDEX   = 18;
    const unsigned int COMMAND_INDEX            = 19;
    const unsigned int DATA_START_INDEX         = 20;
    
    const unsigned int MESSAGE_HEADER_LENGTH    = 21;

}

#endif
