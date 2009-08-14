/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_LASERSCANNER2D_SICK_GBX_H
#define HYDRO_LASERSCANNER2D_SICK_GBX_H

#include <hydrointerfaces/laserscanner2d.h>
#include <gbxsickacfr/driver.h>
#include <memory>

namespace laserscanner2dsickgbx {

//
// @author Alex Brooks
//
class Driver : public hydrointerfaces::LaserScanner2d
{

public: 

    Driver( const Config &config, const hydroutil::Context &context );

    void read( hydrointerfaces::LaserScanner2d::Data &data );

private: 

    std::auto_ptr<gbxsickacfr::Driver> device_;

    Config config_;
    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::LaserScanner2dFactory
{
public:
    hydrointerfaces::LaserScanner2d*
    createDriver( const hydrointerfaces::LaserScanner2d::Config &config,
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
