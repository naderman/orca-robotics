/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alen Alempijevic, Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_IMAGE_DC1394_H
#define HYDRO_IMAGE_DC1394_H

#include <hydrointerfaces/image.h>
#include <dc1394/dc1394.h>

namespace imagedc1394 {

//
// libdc1394-2 image driver
//
class Driver : public hydrointerfaces::Image
{

public:

    Driver( const Config &cfg, const hydroutil::Context &context );
    virtual ~Driver();

    // Blocks til new data is available
    virtual void read( hydrointerfaces::Image::Data &data );

private:

    void cleanup( dc1394camera_t* camera );

    Image::Config config_;
    hydroutil::Context context_;

    dc1394camera_t* 	camera;
    dc1394video_frame_t* frame_;
    dc1394error_t	err_;

    dc1394video_mode_t 	videoMode_;
    unsigned int 	height_;
    unsigned int 	width_;
    bool                format7_;
    dc1394operation_mode_t isoMode_;
    dc1394speed_t       isoSpeed_;
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
