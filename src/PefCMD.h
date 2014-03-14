//
//  PefCMD.h
//  J3NI
//
//  Created by Neil on 2014-03-14.
//  Copyright (c) 2014 Neil. All rights reserved.
//

#ifndef PEFCMD_H
#define PEFCMD_H

#include <I_Command.h>
#include <stdint.h>

class GetPefCapabCMD : public I_Command
{
private:
    // [7] = 0 OEM Event Record Filtering Supported
    // [6] - reserved
    // [5] = 0 (diagnostic interrupt)
    // [4] = 0 (OEM action)
    // [3] = 1 (power cycle)
    // [2] = 1 (reset)
    // [1] = 1 (power down)
    // [0] = 0 (alert)
    unsigned char pefCapab_;
    
public:
    GetPefCapabCMD();
    
    void setPefCapab(unsigned char pefCap);
    unsigned char getPefCapab() const;
    
    virtual int process(const unsigned char* request, int reqLength,
                        unsigned char* response );
    
};


class ArmPefPostponeTimerCMD : public I_Command
{
private:
    time_t setTime;
    short unsigned int countdownDuration;
    
public:
    ArmPefPostponeTimerCMD();
    
    virtual int process(const unsigned char* request, int reqLength,
                        unsigned char* response );
};

//class GetPefConfigParamCMD : public I_Command
//{
//public:
//    virtual int process(const unsigned char* request, int reqLength,
//                        unsigned char* response );
//};
//
//class SetPefConfigParamCMD : public I_Command
//{
//public:
//    virtual int process(const unsigned char* request, int reqLength,
//                        unsigned char* response );
//};

class GetLastProcEventIdCMD : public I_Command
{
private:
    uint16_t* mostRecentId_;
    uint16_t bmcRecordId_;
    uint16_t swRecordId_;
    
public:
    GetLastProcEventIdCMD();
    
    void setBmcRecordId(const unsigned char* recordId, int length);
    void setSwRecordId(const unsigned char* recordId, int length);
    uint16_t getBmcRecordId() const;
    uint16_t getSwRecordId() const;
    
    virtual int process(const unsigned char* request, int reqLength,
                        unsigned char* response );
};

class SetLastProcEventIdCMD : public I_Command
{
private:
    GetLastProcEventIdCMD* lastProcEventCmd_;
    
public:
    SetLastProcEventIdCMD(GetLastProcEventIdCMD* lastProcEventCmd);
    
    virtual int process(const unsigned char* request, int reqLength,
                        unsigned char* response );
};

#endif 
