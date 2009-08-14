/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_DISPARITY_SIMPLE_H
#define HYDRO_DISPARITY_SIMPLE_H

#include <cassert>

#include <hydrointerfaces/disparity.h>

namespace disparitysimple {

//
// An extremely simple, slow, and error prone stereo correspondance driver.
//
class Driver : public hydrointerfaces::Disparity
{

public:

    Driver( const Config &config, const hydroutil::Context &context );
    virtual ~Driver();

    // Computes the disparity of the two given input images
    virtual void process( const Data &leftImage, const Data &rightImage, Data &outputImage );

    virtual bool hasEventLoop() { return false; }
    virtual int executeEventLoop() { assert(false && "Simple Disparity Driver doesn't have an event loop"); return 0; }

private:
    Config config_;
    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::DisparityFactory
{
public:
    hydrointerfaces::Disparity *createDriver( const hydrointerfaces::Disparity::Config &config
					                          , const hydroutil::Context &context
					                        ) const
    {
        return new Driver( config, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::DisparityFactory *createDriverFactory();
}

#endif
