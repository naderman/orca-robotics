/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_LASERSCANNER2D_FAKE_H
#define HYDRO_LASERSCANNER2D_FAKE_H

#include <hydrointerfaces/laserscanner2d.h>

namespace laserscanner2dfake {

//
// Bogus laser driver, useful for testing the rest of the component.
//
class Driver : public hydrointerfaces::LaserScanner2d
{

public:

    Driver( const Config &cfg, const hydroutil::Context &context );
    virtual ~Driver();

    // Blocks till new data is available
    virtual void read( hydrointerfaces::LaserScanner2d::Data &data );

private:

    // configurable sleep interval before returning from read() [milliseconds]
    int sleepIntervalMs_;

    LaserScanner2d::Config config_;
    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::LaserScanner2dFactory
{
public:
    hydrointerfaces::LaserScanner2d *createDriver( 
                const hydrointerfaces::LaserScanner2d::Config &config,
                const hydroutil::Context &context ) const
    {
        return new Driver( config, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::LaserScanner2dFactory *createDriverFactory();
}

#endif
