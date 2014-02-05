//
//  IpmiMessage.h
//  J3NI
//
//  Created by Neil on 2014-01-27.
//  Copyright (c) 2014 J3NI. All rights reserved.
//

#ifndef IPMIMESSAGE_H
#define IPMIMESSAGE_H

class IpmiMessage
{
private:
    int msgLength_;
    unsigned char* message_;
    
public:
    IpmiMessage();
    IpmiMessage(const unsigned char* msg, unsigned int msgSize);
   
    ~IpmiMessage();
    
    bool setMessage(const unsigned char* msg, unsigned int msgLength);
    
    bool serialize(const unsigned char* data, unsigned int dataSize,
                   IpmiMessage& responseMsg) const;

    unsigned char* message() const;
    unsigned int length() const;
    
    unsigned char* data() const;
    
    unsigned char& operator[](const int index);
    const unsigned char& operator[](const int index) const;
 
    
private:
    void clearMessage();
    
    unsigned char computeChecksum(unsigned char* bytes, unsigned int length) const;

};


#endif
