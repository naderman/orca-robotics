/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER2D_FAKE_LASER_DRIVER_H
#define ORCA2_LASER2D_FAKE_LASER_DRIVER_H

#include <orcaice/context.h>
#include "driver.h"


namespace laser2d {

//
// Bogus laser driver, useful for testing the rest of the component.
//
class FakeDriver : public Driver
{

public:

    FakeDriver( const orcaice::Context & context );
    virtual ~FakeDriver();

    virtual int enable() { isEnabled_=true; return 0; };
    virtual int disable() { isEnabled_=false; return 0; };

    virtual bool isEnabled() { return isEnabled_; };

    // Blocks till new data is available
    virtual int read( orca::LaserScanner2dDataPtr &data );

    // Get the current configuration
    virtual int getConfig( Config &cfg );

    // Set a specifc configuration
    virtual int setConfig( const Config &cfg );

private:

    bool isEnabled_;

    orcaice::Context context_;
};

}

#endif
