/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER2D_FAKE_LASER_DRIVER_H
#define ORCA2_LASER2D_FAKE_LASER_DRIVER_H

#include <orcaice/context.h>
#include <laser2dutil/driver.h>


namespace laser2d {

//
// Bogus laser driver, useful for testing the rest of the component.
//
class FakeDriver : public Driver
{

public:

    FakeDriver( const Config &cfg, const orcaice::Context &context );
    virtual ~FakeDriver();

    // Blocks till new data is available
    virtual void read( std::vector<float> &ranges, 
                       std::vector<unsigned char> &intensities, 
                       orca::Time &timeStamp );

private:

    Driver::Config config_;
    orcaice::Context context_;
};

// Used for dynamically loading driver
class FakeDriverFactory : public laser2d::DriverFactory
{
public:
    laser2d::Driver *createDriver( const Driver::Config &config,
                                   const orcaice::Context &context ) const
        {
            return new FakeDriver( config, context );
        }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    laser2d::DriverFactory *createDriverFactory();
}

#endif
