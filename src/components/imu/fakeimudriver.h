/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006 Ben Upcroft
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef FAKE_IMU_DRIVER_H
#define FAKE_IMU_DRIVER_H

#include <iostream>

#include "imudriver.h"

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
    virtual int getData(orca::ImuDataPtr& data );
    virtual int getData(orca::Position3dDataPtr& data );

private:

    bool isEnabled_;
};

#endif
