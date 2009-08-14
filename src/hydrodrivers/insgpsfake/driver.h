/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_INSGPS_FAKE_H
#define HYDRO_INSGPS_FAKE_H

#include <hydrointerfaces/insgps.h>

namespace insgpsfake {

//
// Bogus insgps driver, useful for testing the rest of the component.
//
class Driver : public hydrointerfaces::InsGps
{

public:

    Driver( const Config &cfg, const hydroutil::Context &context );

    // Blocks till new data is available
    virtual std::auto_ptr<hydrointerfaces::InsGps::GenericData> read();

private:

    InsGps::Config config_;
    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::InsGpsFactory
{
public:
    hydrointerfaces::InsGps *createDriver( 
                const hydrointerfaces::InsGps::Config &config,
                const hydroutil::Context &context ) const
    {
        return new Driver( config, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::InsGpsFactory *createDriverFactory();
}

#endif
