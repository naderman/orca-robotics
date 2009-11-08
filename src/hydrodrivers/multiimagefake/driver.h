/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_IMAGECOLLECTION_FAKE_H
#define HYDRO_IMAGECOLLECTION_FAKE_H

#include <hydrointerfaces/multiimage.h>

namespace multiimagefake {

//
// Bogus image driver, useful for testing the rest of the component.
//
class Driver : public hydrointerfaces::MultiImage
{

public:

    Driver( const Config &cfg, const hydroutil::Context &context );
    virtual ~Driver();

    // Blocks till new data is available
    virtual void read( Data &data );

private:
    enum Mode {
        Noise,
        Smooth 
    };
    Mode mode_;
    // alexm: I'm not actually sure what this value is
    unsigned char prevIntensity_;
    Config config_;
    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::MultiImageFactory
{
public:
    hydrointerfaces::MultiImage *createDriver( 
                hydrointerfaces::MultiImage::Config &config,
                const hydroutil::Context &context ) const
    {
        return new Driver( config, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::MultiImageFactory *createMultiImageDriverFactory();
}

#endif
