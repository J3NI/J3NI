//
//  IpmiMessage.h
//  J3NI
//
//  Created by Neil on 2014-01-27.
//  Copyright (c) 2014 J3NI. All rights reserved.
//

#ifndef IPMIMESSAGE_H
#define IPMIMESSAGE_H

#include <stdint.h>

class IpmiMessage
{
private:
    int msgLength_;
    unsigned char* message_;
    
    int dataLength_;
    unsigned char* data_;
    
    uint32_t msgSeqNumber_;;
    uint32_t msgSessionId_;
    
    static uint32_t inboundSequenceNumber_;
    static uint32_t outboundSequenceNumber_;
    static uint32_t sessionId_;
    
    unsigned char command_;
    
public:
    IpmiMessage();
    IpmiMessage(const unsigned char* msg, unsigned int msgSize);
   
    ~IpmiMessage();
    
    bool setMessage(const unsigned char* msg, unsigned int msgLength);
    
    bool serialize(const unsigned char* data, unsigned int dataSize,
                   IpmiMessage& responseMsg) const;
    
    bool validMessage() const;
    
    unsigned char getNetFn() const;

    const unsigned char* message() const;
    unsigned int length() const;
    
    const unsigned char* data() const;
    unsigned int dataLength() const;
    
    unsigned char& operator[](const int index);
    const unsigned char& operator[](const int index) const;
    
    static void setSessionId(const unsigned char* sessionId, unsigned int size);
    
private:
    void clearMessage();
    void updateSessionInfo();
    
    bool validSequenceNumber() const;
    bool isSessionlessCommand() const;
    
    unsigned char computeChecksum(unsigned char* bytes, unsigned int length) const;
};


#endif
