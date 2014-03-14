//
//  PefCMD.cpp
//  J3NI
//
//  Created by Neil on 2014-03-14.
//  Copyright (c) 2014 Neil. All rights reserved.
//

#include <stdlib.h>
#include <time.h>

#include <PefCMD.h>
#include <IpmiCommandDefines.h>

using namespace IpmiCommandDefines;

GetPefCapabCMD::GetPefCapabCMD() : pefCapab_(0x0E)
{
}

void GetPefCapabCMD::setPefCapab(unsigned char pefCap)
{
    pefCapab_ = pefCap;
}

unsigned char GetPefCapabCMD::getPefCapab() const
{
    return pefCapab_;
}

int GetPefCapabCMD::process(const unsigned char* req, int reqLength,
                          unsigned char* resp)
{
    
    resp[0] = COMP_CODE_OK;
    resp[1] = 0x51;
    resp[2] = pefCapab_;
    resp[3] = 0x00;
    
    return 4;
}


ArmPefPostponeTimerCMD::ArmPefPostponeTimerCMD() : countdownDuration(0)
{
    setTime = time(0);
}

int ArmPefPostponeTimerCMD::process(const unsigned char* req, int reqLength,
                                    unsigned char* resp)
{
    
    time_t t = time(0);
    if(req[0] == 0xFE || req[0] == 0x00)
    {
        countdownDuration = 0;
    }
    else if(req[0] != 0xFF)
    {
        countdownDuration = req[0];
        setTime = t;
    }
    unsigned char countdownValue = countdownDuration;
    if(req[0] == 0xFF)
    {
        countdownValue = (t - setTime) % countdownDuration;
    }
    
    resp[0] = COMP_CODE_OK;
    resp[1] = countdownValue;    
    return 2;
}

//int GetPefConfigParamCMD::process(const unsigned char* req, int reqLength,
//                                  unsigned char* resp)
//{
//    resp[0] = COMP_CODE_OK;
//    resp[1] = 0x11;
//    if((req[0] & 0x80) == 0x80)
//        return 2;
//    return 2
//}
//
//int SetPefConfigParamCMD::process(const unsigned char* req, int reqLength,
//                                  unsigned char* resp)
//{
//    resp[0] = COMP_CODE_OK;
//    return 1;
//}


GetLastProcEventIdCMD::GetLastProcEventIdCMD()
    :   mostRecentId_(NULL), bmcRecordId_(0xFFFF), swRecordId_(0xFFFF)
{
    mostRecentId_ = &swRecordId_;
}

void GetLastProcEventIdCMD::setBmcRecordId(const unsigned char* recordId, int length)
{
    if(length < 2) return;
    bmcRecordId_ = recordId[0] | (recordId[1] << 8);
    mostRecentId_ = &bmcRecordId_;
}

void GetLastProcEventIdCMD::setSwRecordId(const unsigned char* recordId, int length)
{
    if(length < 2) return;
    swRecordId_ = recordId[0] | (recordId[1] << 8);
    mostRecentId_ = &swRecordId_;
}

uint16_t GetLastProcEventIdCMD::getBmcRecordId() const
{
    return bmcRecordId_;
}

uint16_t GetLastProcEventIdCMD::getSwRecordId() const
{
    return swRecordId_;
}

int GetLastProcEventIdCMD::process(const unsigned char* req, int reqLength,
                                   unsigned char* resp)
{
    resp[0] = COMP_CODE_OK;
    // 1-4 Timestamp
    resp[5] = *mostRecentId_ & 0x00FF ;
    resp[6] = (*mostRecentId_ & 0xFF00) >> 8;
    resp[7] = swRecordId_ & 0x00FF ;
    resp[8] = (swRecordId_ & 0xFF00) >> 8;
    resp[9] = swRecordId_ & 0x00FF ;
    resp[10] = (swRecordId_ & 0xFF00) >> 8;
}

SetLastProcEventIdCMD::SetLastProcEventIdCMD(GetLastProcEventIdCMD* lastProcEventCmd)
{
    lastProcEventCmd_ = lastProcEventCmd;
}

int SetLastProcEventIdCMD::process(const unsigned char* req, int reqLength,
                                   unsigned char* resp)
{
    resp[0] = COMP_CODE_OK;
    if (reqLength >= 3)
    {
        if((req[0] & 0x01) == 0x01)
            lastProcEventCmd_->setBmcRecordId(req + 1, 2);
        else
            lastProcEventCmd_->setSwRecordId(req + 1, 2);
    }
    return 1;
    
}




