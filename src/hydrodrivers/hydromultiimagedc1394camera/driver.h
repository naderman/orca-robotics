/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alen Alempijevic, Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Michael Warren
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_MULTIIMAGE_DC1394CAMERA_H
#define HYDRO_MULTIIMAGE_DC1394CAMERA_H

#include <hydrointerfaces/multiimage.h>
#include <dc1394/dc1394.h>

namespace multiimagedc1394camera {

//
// libdc1394-2 image driver
//
class Driver : public hydrointerfaces::MultiImage
{

public:

    Driver( const Config &cfg, const hydroutil::Context &context );
    virtual ~Driver();

    // Blocks til new data is available
    virtual void read( hydrointerfaces::MultiImage::Data &data );

private:

    void cleanup( dc1394camera_t* camera );
    
    int grabFrame(int cam_index);
    
    int synchroniseBuffers(uint64_t last_stamp);

    MultiImage::Config config_;
    hydroutil::Context context_;

    std::vector<dc1394camera_t*> camera;
    std::vector<dc1394video_frame_t*> frame_;
    dc1394error_t	err_;

    std::vector<dc1394video_mode_t >	videoMode_;
    std::vector<unsigned int >	height_;
    std::vector<unsigned int >	width_;
    std::vector<bool >               format7_;
    std::vector<dc1394operation_mode_t> isoMode_;
    std::vector<dc1394speed_t >      isoSpeed_;
    std::vector<unsigned int> numBufs;
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
