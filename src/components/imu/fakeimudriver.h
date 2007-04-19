/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef FAKE_IMU_DRIVER_H
#define FAKE_IMU_DRIVER_H

#include <iostream>

#include "imudriver.h"

namespace imu {

//
// Bogus imu driver, useful for testing the rest of the component.
//
class FakeImuDriver : public ImuDriver
{

public:

    FakeImuDriver();
    virtual ~FakeImuDriver();

    virtual int reset(){return 0;};

    virtual int enable() { isEnabled_=true; return 0; };
    virtual int disable() { isEnabled_=false; return 0; };

    virtual bool isEnabled() { return isEnabled_; };

    // Blocks till timout expires, returns number of messages read, -1 if failure
    virtual int read();
    // Fetch latest IMU data. return -1 if unavailable
    virtual int getData(orca::ImuData& data );
    virtual int getData(orca::Odometry3dData& data );
    virtual int getDescription(orca::VehicleDescription& descr ){ return -1; };

private:

    bool isEnabled_;
};

} //namespace

#endif
