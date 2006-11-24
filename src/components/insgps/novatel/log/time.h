/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef _TIME_H_
#define _TIME_H_

#include <novatel/header.h>

namespace novatel{

#pragma pack(push,1)
typedef struct
{
   unsigned long  ulClockModel;   // ClockModelStatus
   double         dGPSOffset;     // Receiver Offset in seconds from GPS time
   double         dOffsetStd;     // Instantaneous Standard Deviation of Receiver Clock Offset
   double         dUtcOffset;     // Offset in seconds of GPS time from UTC time
   long           lUtcYear;       // UTC Year
   unsigned char  ucUtcMonth;     // UTC Month
   unsigned char  ucUtcDay;       // UTC Day
   unsigned char  ucUtcHour;      // UTC Hour
   unsigned char  ucUtcMin;       // UTC Minutes
   long           lUtcMillisec;   // UTC Milliseconds
   int            bUtcStatus;     // UTC Status
} TIME_DATA;

typedef struct
{
   OEM4_BINARY_HEADER   hdr;
   TIME_DATA            data;
} TIMEB_LOG;
#pragma pack(pop)

}//namespace

#endif

