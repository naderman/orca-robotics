/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef _SETIMUTOANTOFFSET_H_
#define _SETIMUTOANTOFFSET_H_

#include <hydronovatelutil/header.h>

namespace hydronovatelutil{

#pragma pack(push,1)
    typedef struct{
	double  x;
	double  y;
	double  z;
	double  x_uncert;
	double  y_uncert;
	double  z_uncert;
    }SETIMUTOANTOFFSET_DATA;

    typedef struct
    {
	OEM4_BINARY_HEADER        hdr;
        SETIMUTOANTOFFSET_DATA    data;
    }SETIMUTOANTOFFSET_CMD;
#pragma pack(pop)

}// namespace

#endif
