/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef _BESTGPSVEL_H_
#define _BESTGPSVEL_H_

#include <hydronovatelutil/header.h>

namespace hydronovatelutil{
#pragma pack(push,1)
    typedef struct{
        unsigned long  sol_status;
        unsigned long  vel_type;
        float          latency;    // [s]? usually either 0, or half the interval (instantious/averaged doppler)
        float          diff_age;   // [s]?
        double         hor_speed;  // [m/s]
        double         heading;    // [deg] w.r. to true north
        double         vert_speed; // [m/s] up positive
        float          reserved;
        unsigned long  crc;
    }BESTGPSVEL_DATA;

    typedef struct BESTGPSVEL_LOG
    {
        OEM4_BINARY_HEADER  hdr;
        BESTGPSVEL_DATA    data;
    }BESTGPSVEL_LOG;
#pragma pack(pop)

}//namespace

#endif

