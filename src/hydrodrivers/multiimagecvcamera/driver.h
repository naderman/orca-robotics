/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *                         Ben Upcroft
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_MULTIIMAGE_CVCAMERA_H
#define HYDRO_MULTIIMAGE_CVCAMERA_H

#include <hydrointerfaces/multiimage.h>
#include <cv.h>
#include <highgui.h>

namespace multiimagecvcamera {

//
// OpenCV camera driver
//
class Driver : public hydrointerfaces::MultiImage
{

public:

    Driver( const Config& cfg, const hydroutil::Context &context );
    virtual ~Driver();

    // Blocks until new data is available
    virtual void read( hydrointerfaces::MultiImage::Data& data );

private:

    // set the frame width and height
    void setWidthAndHeight(int width, int height, int cameraNumber );
    
    MultiImage::Config config_;
    hydroutil::Context context_;

    // opencv structures
    // CvCapture* camera_;
    std::vector<CvCapture*> cameras_;
    std::vector<IplImage*> frames_;
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
    hydrointerfaces::MultiImageFactory *createDriverFactory();
}

#endif
