/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef _INSPVA_H_
#define _INSPVA_H_

#include <novatel/header.h>

namespace novatel{

#pragma pack(push,1)
    typedef struct{
	unsigned long    week;
	double           seconds;
	double           latitude;
	double           longitude;
	double           height;
	double           north_vel;
	double           east_vel;
	double           up_vel;
	double           roll;
	double           pitch;
	double           yaw;
	unsigned long    status;
        unsigned long    crc;
    }INSPVA_DATA;

    typedef struct{
	OEM4_SHORT_BINARY_HEADER   hdr;
        INSPVA_DATA          data;
    }INSPVASB_LOG;

    typedef struct{
	OEM4_BINARY_HEADER   hdr;
        INSPVA_DATA          data;
    }INSPVAB_LOG;
#pragma pack(pop)

}


#endif
