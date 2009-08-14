/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef _RXSTATUS_H_
#define _RXSTATUS_H_

#include <hydronovatelutil/header.h>

namespace hydronovatelutil{

#define MAXSTATS 4       // Maximum number of status codes contained in the log

#pragma pack(push,1)
typedef struct
{
   unsigned long  ulError;                      // Reciever Error
   unsigned long  ulNumStats;                   // Number of Status Codes
} RXSTATUSB_HEADER;

typedef struct
{
   unsigned long  ulRxstat;                  // Reciever Status Word
   unsigned long  ulRxstatPri;               // Reciever Status Priority Mask
   unsigned long  ulRxstatSet;               // Reciever Status Event Set Mask
   unsigned long  ulRxstatClear;             // Reciever Status Event Clear Mask
} RXSTATUSB_DATA;

typedef struct
{
   OEM4_BINARY_HEADER     hdr;
   RXSTATUSB_HEADER       header; 
   RXSTATUSB_DATA         data[MAXSTATS];
} RXSTATUSB_LOG;
#pragma pack(pop)

}//namespace

#endif

