//
//  PefCMD.cpp
//  J3NI
//
//  Created by Neil on 2014-03-14.
//  Copyright (c) 2014 Neil. All rights reserved.
//

#include <fstream>
#include <stdlib.h>
#include <time.h>

#include <PefCMD.h>
#include <IpmiCommandDefines.h>

using namespace IpmiCommandDefines;
extern std::ofstream log_file;

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
    log_file << "Get PEF Capabilities Command" << std::endl;
    
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
    log_file << "Arm PEF Postpone Timer Command" << std::endl;
    
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

GetPefConfigParamCMD::GetPefConfigParamCMD()
{
    unsigned char defaultInProgress = 0x00;
    pefConfigMap[0x00] = new ConfigParam(1, &defaultInProgress);
    
    unsigned char defaultControl = 0x01;
    pefConfigMap[0x01] = new ConfigParam(1, &defaultControl);
    
    unsigned char defaultActionControl = 0x0E;
    pefConfigMap[0x02] = new ConfigParam(1, &defaultActionControl);
    
    unsigned char defaultStartupDelay = 0x00;
    pefConfigMap[0x03] = new ConfigParam(1, &defaultStartupDelay);
    
    unsigned char defaultAlertStartupDelay = 0x00;
    pefConfigMap[0x04] = new ConfigParam(1, &defaultAlertStartupDelay);
    
    unsigned char eventFilters = 0x00;
    pefConfigMap[0x05] = new ConfigParam(1, &eventFilters, true);
    
    unsigned char alertPolicyNum = 0x00;
    pefConfigMap[0x08] = new ConfigParam(1, &alertPolicyNum, true);
    
    unsigned char* guid = new unsigned char[16];
    pefConfigMap[0x0A] = new ConfigParam(16, guid);
    delete [] guid;
    
}


GetPefConfigParamCMD::~GetPefConfigParamCMD()
{
    for(ConfigParamMap::iterator it = pefConfigMap.begin();
        it != pefConfigMap.end(); it++)
    {
        delete it->second;
    }
    pefConfigMap.clear();
}


unsigned char GetPefConfigParamCMD::setMap(unsigned char param,
                                           const unsigned char* paramValue,
                                           int length)
{
    unsigned char paramSelector = paramValue[0] & 0x7F;
    ConfigParamMap::iterator it = pefConfigMap.find(paramSelector);
    if(it == pefConfigMap.end())
    {
        return PARAM_UNSUPPORTED;
    }
    
    ConfigParam* configParam = it->second;
    if(configParam->readOnly)
    {
        return WRITE_TO_READ_ONLY;
    }
    if((param == 0x00) && (configParam->data[0] != 0x00))
    {
        return SET_IN_PROGRESS_FAIL;
    }
    if(configParam->length != length)
    {
        delete[] configParam->data;
        configParam->data = new unsigned char[length];
    }
    for(int i = 0; i < configParam->length; i++)
    {
        configParam->data[i] = paramValue[i];
    }
    return COMP_CODE_OK;
}

int GetPefConfigParamCMD::process(const unsigned char* req, int reqLength,
                                  unsigned char* resp)
{
    log_file << "Get PEF Configuration Parameters Command" << std::endl;
    resp[0] = COMP_CODE_OK;
    resp[1] = 0x11;
    if((req[0] & 0x80) == 0x80)
        return 2;
    
    unsigned char paramSelector = req[0] & 0x7F;
    ConfigParamMap::iterator it = pefConfigMap.find(paramSelector);
    if(it == pefConfigMap.end())
    {
        resp[0] = PARAM_UNSUPPORTED;
        return 2;
    }
    
    ConfigParam* param = it->second;
    int i = 0;
    for(; i < param->length; i++)
    {
        resp[2 + i] = param->data[i];
    }
    return 2 + i;
}


SetPefConfigParamCMD::SetPefConfigParamCMD(GetPefConfigParamCMD* getPefConfigParam)
{
    getPefConfigParam_ = getPefConfigParam;
}

int SetPefConfigParamCMD::process(const unsigned char* req, int reqLength,
                                  unsigned char* resp)
{
    log_file << "Set PEF Configuration Parameters Command" << std::endl;

    unsigned char paramSelector = req[0] & 0x7F;
    resp[0] = getPefConfigParam_->setMap(paramSelector,
                                         req + 1, reqLength - 1);
    
    return 1;
}


GetLastProcEventIdCMD::GetLastProcEventIdCMD()
    :   mostRecentId_(NULL), bmcRecordId_(0x0000), swRecordId_(0x0000)
{
    timestamp_ = time(0);
    mostRecentId_ = &swRecordId_;
}

void GetLastProcEventIdCMD::setBmcRecordId(const unsigned char* recordId, int length)
{
    if(length < 2) return;
    bmcRecordId_ = recordId[0] | (recordId[1] << 8);
    mostRecentId_ = &bmcRecordId_;
    timestamp_ = time(0);
}

void GetLastProcEventIdCMD::setSwRecordId(const unsigned char* recordId, int length)
{
    if(length < 2) return;
    swRecordId_ = recordId[0] | (recordId[1] << 8);
    mostRecentId_ = &swRecordId_;
    timestamp_ = time(0);
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
    log_file << "Get Last Proccessed Event Id Command" << std::endl;
    
    resp[0] = COMP_CODE_OK;
    resp[1] = timestamp_ & 0x000000FF;
    resp[2] = (timestamp_ & 0x0000FF00) >> 8;
    resp[3] = (timestamp_ & 0x00FF0000) >> 16;
    resp[4] = (timestamp_ & 0xFF000000) >> 24;
    resp[5] = *mostRecentId_ & 0x00FF ;
    resp[6] = (*mostRecentId_ & 0xFF00) >> 8;
    resp[7] = swRecordId_ & 0x00FF ;
    resp[8] = (swRecordId_ & 0xFF00) >> 8;
    resp[9] = swRecordId_ & 0x00FF ;
    resp[10] = (swRecordId_ & 0xFF00) >> 8;
    
    return 11; 
}

SetLastProcEventIdCMD::SetLastProcEventIdCMD(GetLastProcEventIdCMD* lastProcEventCmd)
{
    lastProcEventCmd_ = lastProcEventCmd;
}

int SetLastProcEventIdCMD::process(const unsigned char* req, int reqLength,
                                   unsigned char* resp)
{
    log_file << "Set Last Proccessed Event Id Command" << std::endl;
    
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




