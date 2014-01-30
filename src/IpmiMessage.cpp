//
//  IpmiMessage.cpp
//  J3NI
//
//  Created by Neil on 2014-01-27.
//  Copyright (c) 2014 Neil. All rights reserved.
//

#include <exception>
#include <fstream>
#include <stdlib.h>
#include <string.h>

#include <IpmiMessage.h>
#include <IpmiCommandDefines.h>

using namespace IpmiCommandDefines;

extern std::ofstream log_file;

IpmiMessage::IpmiMessage()
:   msgLength_(0), message_(NULL)
{
}

IpmiMessage::IpmiMessage(const unsigned char* msg, unsigned int msgSize)
:   msgLength_(0), message_(NULL)
{
    setMessage(msg, msgSize);
}

IpmiMessage::~IpmiMessage()
{
    clearMessage();
}


bool IpmiMessage::setMessage(const unsigned char* msg, unsigned int msgLength)
{
    if(msgLength < MESSAGE_HEADER_LENGTH)
    {
        return false;
    }
    clearMessage();
    
    msgLength_ = msgLength;
    message_ = new unsigned char[msgLength_];
    for(int i = 0; i < msgLength_; i++)
    {
        message_[i] = msg[i];
    }
    return true;
}

bool IpmiMessage::serialize(const unsigned char* data, unsigned int dataSize,
                            IpmiMessage& responseMsg) const
{
    unsigned int responseSize = dataSize + MESSAGE_HEADER_LENGTH;
    unsigned char* response = new unsigned char[responseSize];
    
    for(int i = 0; i < 4; i++)
    {
        response[RMCP_HEADER_INDEX + i] = message_[RMCP_HEADER_INDEX + i];
        response[SESSION_NUM_INDEX + i] = message_[SESSION_NUM_INDEX + i];
        response[SESSION_ID_INDEX + i] = message_[SESSION_ID_INDEX + i];
    }
    response[AUTH_TYPE_INDEX] = message_[AUTH_TYPE_INDEX];
    
    response[LENGTH_INDEX] = (0x07 + dataSize) & 0xFF;
    response[DEST_ADDRESS_INDEX] = message_[SOURCE_ADDRESS_INDEX];
    
    unsigned char netFn = message_[NET_FN_INDEX] & NET_FN_MASK;
    netFn = (netFn + 0x04) & NET_FN_MASK;
    unsigned char sourceLun = message_[REQUEST_SEQUENCE_INDEX] & LUN_MASK;
    response[NET_FN_INDEX] = netFn | sourceLun;
    
    unsigned char* checksumStart = response + DEST_ADDRESS_INDEX;
    unsigned int checksumLength = 2u;
    response[CHECKSUM_INDEX] = computeChecksum(checksumStart, checksumLength);

    response[SOURCE_ADDRESS_INDEX] = message_[DEST_ADDRESS_INDEX];
    
    unsigned char sequence = message_[REQUEST_SEQUENCE_INDEX] & SEQUENCE_MASK;
    unsigned char responseLun = message_[NET_FN_INDEX] & LUN_MASK;
    response[REQUEST_SEQUENCE_INDEX] = sequence | responseLun;
    response[COMMAND_INDEX] = message_[COMMAND_INDEX];
    
    for(int i = 0; i < dataSize; i++)
    {
        response[DATA_START_INDEX + i] = data[i];
    }
    
    checksumStart = response + SOURCE_ADDRESS_INDEX;
    checksumLength = dataSize + 3u;
    response[responseSize - 1] = computeChecksum(checksumStart, checksumLength);
    
    return responseMsg.setMessage(response, responseSize);
}


unsigned char* IpmiMessage::message() const
{
    return message_;
}

unsigned int IpmiMessage::length() const
{
    return msgLength_;
}

unsigned char& IpmiMessage::operator[](const int index)
{
    return message_[index];
}

const unsigned char& IpmiMessage::operator[](const int index) const
{
    return message_[index];
}


void IpmiMessage::clearMessage()
{
    msgLength_ = 0;
    if(message_ != NULL)
    {
        delete [] message_;
        message_ = NULL;
    }
}

unsigned char IpmiMessage::computeChecksum(unsigned char* bytes,
                                           unsigned int length) const
{
    int checksum = 0;
    for (int i = 0; i < length; i++)
    {
        checksum += bytes[i];
    }
    checksum = ~checksum + 0x1;
    checksum = checksum & 0xFF;
    
    return static_cast<unsigned char>(checksum);
}

