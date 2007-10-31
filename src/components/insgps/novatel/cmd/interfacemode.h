/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef _INTERFACEMODE_H_
#define _INTERFACEMODE_H_

#include <novatel/header.h>

namespace novatel{

    enum INTERFACE_MODES{
	IFMODE_NONE=0,
	IFMODE_NOVATEL,
	IFMODE_RTCM,
	IFMODE_RTCA,
	IFMODE_CMR,
        IFMODE_OMNISTAR,
	IFMODE_IMU=7,
	IFMODE_RTCMNOCR,
        IFMODE_CDGPS
    };

#pragma pack(push,1)
    typedef struct{
	unsigned long port;
	unsigned long rxtype;
	unsigned long txtype;
	unsigned long responses;
    }INTERFACEMODE_DATA;

    typedef struct{
	OEM4_BINARY_HEADER hdr;
        INTERFACEMODE_DATA data;
    }INTERFACEMODE_CMD;
#pragma pack(pop)

}//namespace

#endif

