/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Duncan Mercer, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_GPS_GARMIN_GBX_H
#define HYDRO_GPS_GARMIN_GBX_H

#include <hydrointerfaces/gps.h>
#include <gbxgarminacfr/driver.h>
#include <memory>

namespace gpsgarmingbx {

class Driver : public hydrointerfaces::Gps
{

public:

    Driver( const hydrointerfaces::Gps::Config &cfg, const hydroutil::Context &context );

    // Blocks till new data is available
    virtual void read( hydrointerfaces::Gps::Data &data );

private:

    std::auto_ptr<gbxgarminacfr::Driver> device_;

    hydrointerfaces::Gps::Config config_;
    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::GpsFactory
{
public:
    hydrointerfaces::Gps *createDriver( 
                const hydrointerfaces::Gps::Config &config,
                const hydroutil::Context &context ) const
    {
        return new Driver( config, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::GpsFactory *createDriverFactory();
}

#endif
