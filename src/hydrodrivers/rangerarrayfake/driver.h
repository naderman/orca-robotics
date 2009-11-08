/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_RANGERARRAY_FAKE_H
#define HYDRO_RANGERARRAY_FAKE_H

#include <hydrointerfaces/rangerarray.h>

namespace rangerarrayfake {

//
// Bogus ranger array driver, useful for testing the rest of the component.
//
class Driver : public hydrointerfaces::RangerArray
{

public:

    Driver( const hydroutil::Context &context );
    virtual ~Driver();
    
    // Geometry access
    virtual void getDescription( hydrointerfaces::RangerArray::Description &description );

    // Blocks till new data is available
    virtual void read( hydrointerfaces::RangerArray::Data &data );

private:

    // configurable sleep interval before returning from read() [milliseconds]
    int sleepIntervalMs_;

    RangerArray::Description description_;
    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::RangerArrayFactory
{
public:
    hydrointerfaces::RangerArray *createDriver( 
                const hydroutil::Context &context ) const
    {
        return new Driver( context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::RangerArrayFactory *createRangerArrayDriverFactory();
}

#endif
