//
//  IpmiMessage.cpp
//  J3NI
//
//  Created by Neil on 2014-01-27.
//  Copyright (c) 2014 J3NI. All rights reserved.
//

#include <exception>
#include <fstream>
#include <stdlib.h>
#include <string.h>

#include <IpmiMessage.h>
#include <IpmiCommandDefines.h>

using namespace IpmiCommandDefines;

extern std::ofstream log_file;

uint32_t IpmiMessage::inboundSequenceNumber_ = 0;
uint32_t IpmiMessage::outboundSequenceNumber_ = 0;
uint32_t IpmiMessage::sessionId_ = 0;

IpmiMessage::IpmiMessage()
:   msgLength_(0), message_(NULL), dataLength_(0), data_(NULL)
{
}

IpmiMessage::IpmiMessage(const unsigned char* msg, unsigned int msgSize)
:   msgLength_(0), message_(NULL), dataLength_(0), data_(NULL)
{
    setMessage(msg, msgSize);
}

IpmiMessage::~IpmiMessage()
{
    clearMessage();
}


bool IpmiMessage::setMessage(const unsigned char* msg, unsigned int msgLength)
{
    if(msgLength < MESSAGE_HEADER_LENGTH && msgLength != PING_LENGTH)
    {
        return false;
    }
    clearMessage();
 
    updateSessionInfo();
    
    command_ = message_[COMMAND_INDEX];

    msgLength_ = msgLength;
    message_ = new unsigned char[msgLength_];
    for(int i = 0; i < msgLength_; i++)
    {
        message_[i] = msg[i];
    }
    
    dataLength_ = msgLength_ - (DATA_START_INDEX + 1);
    data_ = new unsigned char[dataLength_];
    for (int i = 0; i < dataLength_; i++)
    {
        data_[i] = message_[i + DATA_START_INDEX];
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
        response[SESSION_ID_INDEX + i] = message_[SESSION_ID_INDEX + i];
    }
    if(sessionId_ != 0)
    {
        outboundSequenceNumber_++;
        response[SESSION_SEQ_NUM_INDEX] = (outboundSequenceNumber_ & 0xFF000000) >> 24;
        response[SESSION_SEQ_NUM_INDEX + 1] = (outboundSequenceNumber_ & 0x00FF0000) >> 16;
        response[SESSION_SEQ_NUM_INDEX + 2] = (outboundSequenceNumber_ & 0x0000FF00) >> 8;
        response[SESSION_SEQ_NUM_INDEX + 3] = (outboundSequenceNumber_ & 0x000000FF);
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
    response[COMMAND_INDEX] = command_;
    
    for(int i = 0; i < dataSize; i++)
    {
        response[DATA_START_INDEX + i] = data[i];
    }
    
    checksumStart = response + SOURCE_ADDRESS_INDEX;
    checksumLength = dataSize + 3u;
    response[responseSize - 1] = computeChecksum(checksumStart, checksumLength);
    
    return responseMsg.setMessage(response, responseSize);
}

bool IpmiMessage::validMessage() const
{
    if(msgSessionId_ == 0u)
        return isSessionlessCommand();
    else
        return ((sessionId_ == msgSessionId_) && validSequenceNumber());
}

const unsigned char* IpmiMessage::message() const
{
    return message_;
}

unsigned int IpmiMessage::length() const
{
    return msgLength_;
}

const unsigned char* IpmiMessage::data() const
{
    return data_;
}

unsigned int IpmiMessage::dataLength() const
{
    return dataLength_;
}


unsigned char& IpmiMessage::operator[](const int index)
{
    return message_[index];
}

const unsigned char& IpmiMessage::operator[](const int index) const
{
    return message_[index];
}

void IpmiMessage::setSessionId(const unsigned char* sessionId,
                               unsigned int size)
{
    if(size < 4) return;
    
    sessionId_ = sessionId[0] << 24;
    sessionId_ = sessionId_ | (sessionId[1] << 16);
    sessionId_ = sessionId_ | (sessionId[2] << 8);
    sessionId_ = sessionId_ | sessionId[3];
}



void IpmiMessage::clearMessage()
{
    msgLength_ = 0;
    if(message_ != NULL)
    {
        delete [] message_;
        message_ = NULL;
    }
    
    dataLength_ = 0;
    if(data_ != NULL)
    {
        delete [] data_;
        data_ = NULL;
    }
}

void IpmiMessage::updateSessionInfo()
{
    msgSessionId_ = message_[SESSION_ID_INDEX] << 24;
    msgSessionId_ = msgSessionId_ | (message_[SESSION_ID_INDEX + 1] << 16);
    msgSessionId_ = msgSessionId_ | (message_[SESSION_ID_INDEX + 2] << 8);
    msgSessionId_ = msgSessionId_ | (message_[SESSION_ID_INDEX + 3]);
    
    msgSeqNumber_ = message_[SESSION_SEQ_NUM_INDEX] << 24;
    msgSeqNumber_ = msgSeqNumber_ |
        (message_[SESSION_SEQ_NUM_INDEX + 1] << 16);
    msgSeqNumber_ = msgSeqNumber_ |
        (message_[SESSION_SEQ_NUM_INDEX + 2] << 8);
    msgSeqNumber_ = msgSeqNumber_ |
        (message_[SESSION_SEQ_NUM_INDEX + 3]);
    
    if(sessionId_ != 0 && sessionId_ == msgSessionId_)
    {
        if((msgSeqNumber_ > inboundSequenceNumber_) &&
           (msgSeqNumber_ <= inboundSequenceNumber_ + 16))
        {
            inboundSequenceNumber_ = msgSeqNumber_;
        }
    }
}

bool IpmiMessage::validSequenceNumber() const
{
    return ((msgSeqNumber_ >= inboundSequenceNumber_ - 16) &&
            (msgSeqNumber_ <= inboundSequenceNumber_ + 15));
}

bool IpmiMessage::isSessionlessCommand() const
{
    bool retVal = false;
    switch (command_)
    {
        case 0x38:
        case 0x39:
        case 0x3a:
            retVal = true;
            break;
        default:
            break;
    }
    return retVal;
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

