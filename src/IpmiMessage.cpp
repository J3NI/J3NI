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

unsigned char IpmiMessage::password_[16] = {0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

IpmiMessage::IpmiMessage()
:   msgLength_(0), message_(NULL), dataLength_(0), data_(NULL),
    authCodeSize_(0), command_(0x00)
{
}

IpmiMessage::IpmiMessage(const unsigned char* msg, unsigned int msgSize)
:   msgLength_(0), message_(NULL), dataLength_(0), data_(NULL),
    authCodeSize_(0), command_(0x00)
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
    
    msgLength_ = msgLength;
    message_ = new unsigned char[msgLength_];
    for(int i = 0; i < msgLength_; i++)
    {
        message_[i] = msg[i];
    }
    
    if(message_[AUTH_TYPE_INDEX] == 0x04)
    {
        authCodeSize_ = AUTH_CODE_LENGTH;
    }
    
    if(msgLength_ < MESSAGE_HEADER_LENGTH + authCodeSize_)
    {
        return true;
    }
    
    updateSessionInfo();
    
    command_ = message_[COMMAND_INDEX + authCodeSize_];
  
    dataLength_ = msgLength_ - (DATA_START_INDEX + authCodeSize_ + 1);
    data_ = new unsigned char[dataLength_];
    for (int i = 0; i < dataLength_; i++)
    {
        data_[i] = message_[i + DATA_START_INDEX + authCodeSize_];
    }
    
    return true;
}


bool IpmiMessage::serialize(const unsigned char* data, unsigned int dataSize,
                            IpmiMessage& responseMsg) const
{
    unsigned int responseSize = dataSize + MESSAGE_HEADER_LENGTH + authCodeSize_;
    unsigned char* response = new unsigned char[responseSize];
    
    for(int i = 0; i < 4; i++)
    {
        response[RMCP_HEADER_INDEX + i] = message_[RMCP_HEADER_INDEX + i];
        response[SESSION_ID_INDEX + i] = message_[SESSION_ID_INDEX + i];
    }
    if(sessionId_ != 0)
    {
        outboundSequenceNumber_++;
        response[SESSION_SEQ_NUM_INDEX] = (outboundSequenceNumber_ & 0x000000FF);
        response[SESSION_SEQ_NUM_INDEX + 1] = (outboundSequenceNumber_ & 0x0000FF00) >> 8;
        response[SESSION_SEQ_NUM_INDEX + 2] = (outboundSequenceNumber_ & 0x00FF0000) >> 16;
        response[SESSION_SEQ_NUM_INDEX + 3] = (outboundSequenceNumber_ & 0xFF000000) >> 24;
    }
    
    response[AUTH_TYPE_INDEX] = message_[AUTH_TYPE_INDEX];
    for(int i = 0; i < authCodeSize_; i++)
    {
        response[AUTH_CODE_INDEX + i] = password_[i];
    }
    
    response[LENGTH_INDEX + authCodeSize_] = (0x07 + dataSize) & 0xFF;
    response[DEST_ADDRESS_INDEX + authCodeSize_] = message_[SOURCE_ADDRESS_INDEX + authCodeSize_];
    
    unsigned char netFn = message_[NET_FN_INDEX + authCodeSize_] & NET_FN_MASK;
    netFn = (netFn + 0x04) & NET_FN_MASK;
    unsigned char sourceLun = message_[REQUEST_SEQUENCE_INDEX + authCodeSize_] & LUN_MASK;
    response[NET_FN_INDEX + authCodeSize_] = netFn | sourceLun;
    
    unsigned char* checksumStart = response + DEST_ADDRESS_INDEX + authCodeSize_;
    unsigned int checksumLength = 2u;
    response[CHECKSUM_INDEX + authCodeSize_] = computeChecksum(checksumStart, checksumLength);

    response[SOURCE_ADDRESS_INDEX + authCodeSize_] = message_[DEST_ADDRESS_INDEX + authCodeSize_];
    
    unsigned char sequence = message_[REQUEST_SEQUENCE_INDEX + authCodeSize_] & SEQUENCE_MASK;
    unsigned char responseLun = message_[NET_FN_INDEX + authCodeSize_] & LUN_MASK;
    response[REQUEST_SEQUENCE_INDEX + authCodeSize_] = sequence | responseLun;
    response[COMMAND_INDEX + authCodeSize_] = command_;
    
    for(int i = 0; i < dataSize; i++)
    {
        response[DATA_START_INDEX + i + authCodeSize_] = data[i];
    }
    
    checksumStart = response + SOURCE_ADDRESS_INDEX + authCodeSize_;
    checksumLength = dataSize + 3u;
    response[responseSize - 1] = computeChecksum(checksumStart, checksumLength);
    
    return responseMsg.setMessage(response, responseSize);
}

bool IpmiMessage::validateSession() const
{
    if(msgSessionId_ == 0u)
        return isSessionlessCommand();
    else
        return ((sessionId_ == msgSessionId_) && validSequenceNumber());
}

bool IpmiMessage::validateAuthCode() const
{
    for(int i = 0; i < authCodeSize_; i++)
    {
        if(message_[AUTH_CODE_INDEX + i] != password_[i])
        {
            return false;
        }
    }
    
    return true;
}

unsigned char IpmiMessage::getCommand() const
{
    return command_;
}


unsigned char IpmiMessage::getNetFn() const
{
    return ((message_[NET_FN_INDEX + authCodeSize_] & NET_FN_MASK) >> 2);
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
    
    inboundSequenceNumber_ = 0;
    outboundSequenceNumber_ = 0;
    sessionId_ = sessionId[0] << 24;
    sessionId_ = sessionId_ | (sessionId[1] << 16);
    sessionId_ = sessionId_ | (sessionId[2] << 8);
    sessionId_ = sessionId_ | sessionId[3];
}

void IpmiMessage::setPassword(const char* password)
{
    if(password != NULL)
    {
        for(int i = 0; (i < 16) && (password[i] != '\0'); i++)
        {
            password_[i] = password[i];
        }
    }
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
    
    msgSeqNumber_ = message_[SESSION_SEQ_NUM_INDEX];
    msgSeqNumber_ = msgSeqNumber_ |
        (message_[SESSION_SEQ_NUM_INDEX + 1] << 8);
    msgSeqNumber_ = msgSeqNumber_ |
        (message_[SESSION_SEQ_NUM_INDEX + 2] << 16);
    msgSeqNumber_ = msgSeqNumber_ |
        (message_[SESSION_SEQ_NUM_INDEX + 3] << 24);
    
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
    uint32_t minSeqNum = inboundSequenceNumber_ - 16;
    uint32_t maxSeqNum = inboundSequenceNumber_ + 15;

    if((minSeqNum > inboundSequenceNumber_) ||
       (maxSeqNum < inboundSequenceNumber_))
    {   
        return ((msgSeqNumber_ <= maxSeqNum) ||
                (msgSeqNumber_ >= minSeqNum));
    }

    return ((msgSeqNumber_ >= minSeqNum) &&
            (msgSeqNumber_ <= maxSeqNum));
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

