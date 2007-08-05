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

    virtual void init() {};

    virtual bool hasFix();

    virtual void read();

    virtual int getData(orca::GpsData& data );

    virtual int getTimeData(orca::GpsTimeData& data );

private:

    std::vector<double> latitudes_;
    std::vector<double> longitudes_;
    unsigned int numReads_;
    unsigned int dataRecord_;
    int fixCounter_;
    
};

}

#endif
