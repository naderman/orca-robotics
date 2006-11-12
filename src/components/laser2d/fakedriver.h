/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef FAKE_LASER_DRIVER_H
#define FAKE_LASER_DRIVER_H

#include "driver.h"

#include <iostream>


namespace sicklaser {

//
// Bogus laser driver, useful for testing the rest of the component.
//
class FakeDriver : public Driver
{

public:

    FakeDriver();
    virtual ~FakeDriver();

    virtual int enable() { isEnabled_=true; return 0; };
    virtual int disable() { isEnabled_=false; return 0; };

    virtual bool isEnabled() { return isEnabled_; };

    // Blocks till new data is available
    virtual int read( orca::LaserScanner2dDataPtr &data );

    // Set a specifc configuration
    virtual int setConfig( const orca::RangeScanner2dConfigPtr &cfg );

    // Get the current configuration
    virtual int getConfig( orca::RangeScanner2dConfigPtr &cfg );

private:

    bool isEnabled_;
    
    orca::RangeScanner2dConfigPtr currentConfig_;
};

}

#endif
