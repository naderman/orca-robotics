/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_IMAGE_FILE_H
#define HYDRO_IMAGE_FILE_H

#include <vector>
#include <string>

#include <hydrointerfaces/image.h>

#include <QImage>

namespace imagefile {

//
// Bogus image driver that reads from a file, useful for testing the rest of the component.
//
class Driver : public hydrointerfaces::Image
{

public:

    Driver( Config &cfg, const hydroutil::Context &context );
    virtual ~Driver();

    // Blocks till new data is available
    virtual void read( hydrointerfaces::Image::Data &data );
    
private:

    Image::Config config_;
    hydroutil::Context context_;
    std::string filename_;
    QImage image_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::ImageFactory
{
public:
    hydrointerfaces::Image *createDriver( 
                hydrointerfaces::Image::Config &config,
                const hydroutil::Context &context ) const
    {
        return new Driver( config, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::ImageFactory *createDriverFactory();
}

#endif
