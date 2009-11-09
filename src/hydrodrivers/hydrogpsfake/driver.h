/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_GPS_FAKE_H
#define HYDRO_GPS_FAKE_H

#include <hydrointerfaces/gps.h>

namespace gpsfake {

//
// Bogus driver, useful for testing the rest of the component.
//
class Driver : public hydrointerfaces::Gps
{

public:

    Driver( const Config &cfg, const hydroutil::Context &context );

    virtual void read( hydrointerfaces::Gps::Data &data );

private:

    std::vector<double> latitudes_;
    std::vector<double> longitudes_;
    unsigned int numReads_;
    unsigned int dataRecord_;
    int fixCounter_;

    bool neverSeeSatellites_;

    Gps::Config config_;
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
    hydrointerfaces::GpsFactory *createGpsDriverFactory();
}

#endif
