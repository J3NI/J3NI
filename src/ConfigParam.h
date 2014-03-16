//
//  ConfigParam.h
//  J3NI
//
//  Created by Neil on 2014-03-14.
//  Copyright (c) 2014 J3NI. All rights reserved.
//

#ifndef CONFIGPARAM_H
#define CONFIGPARAM_H

#include <map>

struct ConfigParam
{
    int rev;
    int length;
    unsigned char * data;
    bool readOnly;
    
    ConfigParam(int l, unsigned char* d, bool ro = false)
    {
        rev=0;
        length = l;
        data = new unsigned char[length];
        for (int i = 0; i < l; i++) data[i] = d[i];
        readOnly = ro;
    }
    
    ConfigParam(){
        delete data;
    }
};

typedef std::map<unsigned char, ConfigParam*> ConfigParamMap;

#endif
