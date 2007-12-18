/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HANDLER_H
#define HANDLER_H

#include <orca/imu.h>
#include <orca/bros1.h>
#include <hydroutil/safethread.h>
#include <orcaice/context.h>
// #include <hydronavutil/mapgrid.h>

#include "imudriver.h"
#include "imuI.h"
#include <orcaifaceimpl/odometry3dImpl.h>

namespace imu {

//
// @brief the main handler of this IMU component.
//
// Note: this thing self-destructs when run() returns.
//
class MainThread : public hydroutil::SafeThread
{

public:

    MainThread(ImuI                            &imuObj,
               orcaifaceimpl::Odometry3dImpl  &odometry3dObj,
	           ImuDriver                       *hwDriver,
                   orca::CartesianPoint            frameOffset,
	           orcaice::Context                current,
	           bool                            startEnabled );
    ~MainThread();

    virtual void walk();

private:
    ImuI& imuObj_;
    orcaifaceimpl::Odometry3dImpl& odometry3dObj_;
    // Generic driver for the hardware
    ImuDriver* hwDriver_;
    const orca::CartesianPoint frameOffset_;

    // mgaMapgrid mgaMapgrid_;

    orcaice::Context context_;
};

} // namespace

#endif
