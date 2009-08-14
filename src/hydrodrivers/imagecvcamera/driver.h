/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_IMAGE_CVCAMERA_H
#define HYDRO_IMAGE_CVCAMERA_H

#include <hydrointerfaces/image.h>
#include <cv.h>
#include <highgui.h>

namespace imagecvcamera {

//
// OpenCV camera driver
//
class Driver : public hydrointerfaces::Image
{

public:

    Driver( const Config &cfg, const hydroutil::Context &context );
    virtual ~Driver();

    // Blocks til new data is available
    virtual void read( hydrointerfaces::Image::Data &data );

private:

    // set the frame width and height
    void setWidthAndHeight(int width, int height);
    
    Image::Config config_;
    hydroutil::Context context_;

    // opencv structures
    CvCapture* camera_;
    IplImage* frame_;
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
