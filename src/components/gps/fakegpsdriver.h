/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef FAKE_GPS_DRIVER_H
#define FAKE_GPS_DRIVER_H

#include <iostream>

#include "driver.h"

namespace gps {

//
// Bogus gps driver, useful for testing the rest of the component.
//
class FakeGpsDriver : public Driver
{

public:

    FakeGpsDriver(std::vector<double> &latitudes, std::vector<double> &longitudes);
    virtual ~FakeGpsDriver();

    virtual int reset(){return 0;};

    virtual int enable() { isEnabled_=true; return 0; };
    virtual int disable() { isEnabled_=false; return 0; };

    virtual bool isEnabled() { return isEnabled_; };
    virtual bool hasFix();

    // Blocks till timout expires, returns number of messages read, -1 if failure
    virtual int read();
    // Fetch latest GPS data. return -1 if unavailable
    virtual int getData(orca::GpsData& data );
    // Fetch latest GPS Time information. return -1 if unavailable
    virtual int getTimeData(orca::GpsTimeData& data );

private:

    bool isEnabled_;
    std::vector<double> latitudes_;
    std::vector<double> longitudes_;
    unsigned int numReads_;
    unsigned int dataRecord_;
    int fixCounter_;
    
};

}

#endif
