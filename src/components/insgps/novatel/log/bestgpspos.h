/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef _BESTGPSPOS_H_
#define _BESTGPSPOS_H_

#include <novatel/header.h>

namespace novatel{

    enum POS_VEL_TYPE{
	NONE=0,
        FIXEDPOS=1,
        FIXEDHEIGHT=2,
        FLOATCONV=4,
        WIDELANE=5,
        NARROWLANE=6,
        DOPPLER_VELOCITY=8,
        SINGLE=16,
        PSRDIFF=17,
        WAAS=18,
        PROPOGATED=19,
        OMNISTAR=20,
        L1_FLOAT=32,
        IONFREE_FLOAT=33,
        NARROW_FLOAT=34,
        L1_INT=48,
        WIDE_INT=49,
        NARROW_INT=50,
        RTK_DIRECT_INS=51,
        INS=52,
        INS_PSRSP=53,
        INS_PSRFLOAT=54,
        INS_RTKFLOAT=55,
        INS_RTKFIXED=56,
        OMNISTAR_HP=64,

    };

    enum SOL_STATUS{
	SOL_COMPUTED=0,
	INSUFFICIENT_OBS=1,
	NO_CONVERGENCE=2,
	SINGULARITY=3,
	COV_TRACE=4,
	TEST_DIST=5,
	COLD_START=6,
	V_H_LIMIT=7,
	VARIANCE=8,
	RESIDUALS=9,
	DELTA_POS=10,
	NEGATIVE_VAR=11,
	INTEGRITY_WARNING=13,
	INS_INACTIVE=14,
	INS_ALIGNING=15,
	INS_BAD=16,
	IMU_UNPLUGGED=17,
	PENDING=18,
	INVALID_FIX=19,

    };

#pragma pack(push,1)
    typedef struct{
	unsigned long  sol_status;
	unsigned long  pos_type;
	double         latitude;
	double         longitude;
	double         height;
	float          undulation;
	unsigned long  datum_id;
	float          lat_sigma;
	float          lon_sigma;
	float          hgt_sigma;
	char           station_id[4];
	float          diff_age;
	float          sol_age;
	unsigned char  num_obs;
	unsigned char  num_GPSL1;
	unsigned char  num_L1;
	unsigned char  num_L2;
	unsigned char  reserved0;
	unsigned char  reserved1;
	unsigned char  reserved2;
	unsigned char  reserved3;
        unsigned long  crc; //again ??
    }BESTGPSPOS_DATA;

    typedef struct
    {
        OEM4_BINARY_HEADER  hdr;
        BESTGPSPOS_DATA    data;
    }BESTGPSPOS_LOG;
#pragma pack(pop)

}//namespace

#endif

