/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef _SETIMUTYPE_H_
#define _SETIMUTYPE_H_

#include <novatel/header.h>

namespace novatel{

    typedef enum IMU_TYPES{
	IMU_UNKNOWN=0,
	IMU_HG1700_AG11,
	IMU_HG1700_AG17=4
    }

#pragma pack(push,1)
    typedef struct
    {
	OEM4_BINARY_HEADER hdr;
        unsigned long  type;
    }SETIMUTYPE_CMD;
#pragma pack(pop)

}//namespace

#endif
