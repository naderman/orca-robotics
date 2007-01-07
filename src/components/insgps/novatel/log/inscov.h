/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef _INSCOV_H_
#define _INSCOV_H_

#include <novatel/header.h>

namespace novatel{
#pragma pack(push,1)
    typedef struct{
	unsigned long    week;
	double           seconds;
	double           pos_cov[9];
	double           att_cov[9];
	double           vel_cov[9];
        unsigned long    crc;
    }INSCOV_DATA;

    typedef struct{
	OEM4_BINARY_HEADER   hdr;
        INSCOV_DATA          data;
    }INSCOVB_LOG;

    typedef struct{
	OEM4_SHORT_BINARY_HEADER   hdr;
        INSCOV_DATA          data;
    }INSCOVSB_LOG;
#pragma pack(pop)

}


#endif
