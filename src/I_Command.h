//
//  I_Command.h
//  J3NI
//
//  Created by Neil on 2014-02-19.
//  Copyright (c) 2014 Neil. All rights reserved.
//

#ifndef I_COMMAND_H
#define I_COMMAND_H

class I_Command{
public:
    I_Command() {};
    virtual ~I_Command() {};
    
    virtual int process(const unsigned char* request, int reqLength,
                        unsigned char* response ) = 0;
    
};

#endif
