/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef _RAWIMU_H_
#define _RAWIMU_H_

#include <hydronovatelutil/header.h>

namespace hydronovatelutil{

    const long IMU_STATUS_CNT   = 0x0000000f;
    const long IMU_STATUS_TEST  = 0x00000010;
    const long IMU_STATUS_ZGYRO = 0x00000020;
    const long IMU_STATUS_YGYRO = 0x00000040;
    const long IMU_STATUS_XGYRO = 0x00000080;

    const long IMU_STATUS_TEMP  = 0x0000ff00;
    const long IMU_STATUS_VER   = 0x00ff0000;

    const long IMU_STATUS_GYRO  = 0x08000000;
    const long IMU_STATUS_ACC   = 0x10000000;
    const long IMU_STATUS_OTHER = 0x20000000;
    const long IMU_STATUS_MEM   = 0x40000000;
    const long IMU_STATUS_PROC  = 0x80000000;

    // TODO: these need to be double checked! Values, units, same for different IMU type??
    const double IMU_GYRO_CONST = (1.1642e-10); // yields change in angle in [degree]; divide by dt to get rate
    const double IMU_ACCEL_CONST = (1.4901e-8*12.0*0.0254); // yields change in velocity [m/s]; divide by dt to get acc


#pragma pack(push,1)
    typedef struct{
	unsigned long    week;
	double           seconds;
	long             status;
	long             z_accel;
	long             y_accel;
	long             x_accel;
	long             z_gyro;
	long             y_gyro;
	long             x_gyro;
        unsigned long    crc;
    }RAWIMU_DATA;

    typedef struct{
	OEM4_BINARY_HEADER   hdr;
        RAWIMU_DATA          data;
    }RAWIMUB_LOG;

    typedef struct RAWIMUSB_LOG{
	OEM4_SHORT_BINARY_HEADER   hdr;
        RAWIMU_DATA          data;
    }RAWIMUSB_LOG;

#pragma pack(pop)

}//namespace

#endif
