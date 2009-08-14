/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef _INSCOMMAND_H_
#define _INSCOMMAND_H_

#include <hydronovatelutil/header.h>

namespace hydronovatelutil{

#pragma pack(push,1)
    typedef struct
    {
	OEM4_BINARY_HEADER hdr;
        unsigned long  action;
    }INSCOMMAND_CMD;
#pragma pack(pop)

}//namespace

#endif
