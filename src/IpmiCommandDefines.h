//
//  IpmiCommandDefines.h
//  J3NI
//
//  Copyright (c) 2014 J3NI. All rights reserved.
//

#ifndef IPMICOMMANDDEFINES_H
#define IPMICOMMANDDEFINES_H

namespace IpmiCommandDefines
{
    const unsigned char CHALLENGE_STRING[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    const unsigned char TEMP_SESSION_ID[4] = {0x01, 0xff, 0x10, 0x53};
    
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
    const unsigned int SESSION_SEQ_NUM_INDEX    = 5;
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
    
    const unsigned int COMP_CODE_OK             = 0x00;
    const unsigned int CANNOT_EXEC_IN_CUR_STATE = 0xD5;
    const unsigned int CANNOT_EXEC_DISABLED     = 0xD6;
    const unsigned int INVALID_SESSION_ID       = 0x85;
    const unsigned int PRIV_LVL_UNAVAILABLE     = 0x80;
    const unsigned int PARAM_UNSUPPORTED        = 0x80;
    const unsigned int ACC_MODE_UNSUPPORTED     = 0x83;
    const unsigned int UNKNOWN_ERROR            = 0xFF;
    
    const unsigned int MAX_DATA_SIZE            = 42;
    
    const unsigned int CHANNEL_NUMBER           = 0x01;
}

#endif
