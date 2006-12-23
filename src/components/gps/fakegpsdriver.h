/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
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
#ifndef FAKE_GPS_DRIVER_H
#define FAKE_GPS_DRIVER_H

#include <iostream>

#include "gpsdriver.h"

//
// Bogus laser driver, useful for testing the rest of the component.
//
class FakeGpsDriver : public GpsDriver
{

public:

    FakeGpsDriver();
    virtual ~FakeGpsDriver();

    virtual int reset(){return 0;};

    virtual int enable() { isEnabled_=true; return 0; };
    virtual int disable() { isEnabled_=false; return 0; };

    virtual bool isEnabled() { return isEnabled_; };
    virtual bool hasFix() { return hasFix_; };

    // Blocks till timout expires, returns number of messages read, -1 if failure
    virtual int read();
    // Fetch latest GPS data. return -1 if unavailable
    virtual int getData(orca::GpsData& data );
    // Fetch latest GPS Time information. return -1 if unavailable
    virtual int getTimeData(orca::GpsTimeData& data );

private:

    bool isEnabled_;
};

#endif
