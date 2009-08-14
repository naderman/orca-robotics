/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef _TIMESYNC_H_
#define _TIMESYNC_H_

#include <hydronovatelutil/header.h>

namespace hydronovatelutil{

#pragma pack(push,1)
typedef struct
{
   unsigned long  week;     // GPS week
   unsigned long  mseconds; // milliseconds in GPS week
   unsigned long  status;   // Status
   unsigned long  crc;
} TIMESYNC_DATA;

typedef struct
{
   OEM4_BINARY_HEADER   hdr;
   TIMESYNC_DATA        data;
} TIMESYNCB_LOG;
#pragma pack(pop)

}//namespace

#endif

