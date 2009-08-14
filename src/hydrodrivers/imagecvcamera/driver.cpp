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

#include <iostream>
#include <sstream>
#include <cmath>
#include <IceUtil/IceUtil.h>
#include <dc1394/types.h>

#include <gbxutilacfr/exceptions.h>

#include "driver.h"

using namespace std;

namespace imagecvcamera {

Driver::Driver( const Config& cfg, const hydroutil::Context& context ) :
    config_(cfg),
    context_(context)
{
    // picks up the first camera it finds on the firewire bus
    camera_ = cvCreateCameraCapture( CV_CAP_ANY );
    if( camera_ == NULL ) {
        std::string ss = "Failed to initialize CvCameraDevice";
        gbxutilacfr::Exception( ERROR_INFO, ss );
        throw ss;
    }
    
    // the default in opencv is CV_CAP_PROP_CONVERT_RGB=1 which causes
    // cameras with bayer encoding to be converted from mono to rgb
    // without using the bayer functions. CV_CAP_PROP_CONVERT_RGB=0
    // keeps the original format.
    cvSetCaptureProperty( camera_, CV_CAP_PROP_CONVERT_RGB, 0 );

    setWidthAndHeight(static_cast<int>(config_.width), static_cast<int>(config_.height) );
    
    // record the actual width and height
    frame_ = cvQueryFrame( camera_ );
    int width = frame_->width;
    int height = frame_->height;

    // this following call should work in opencv but have had mixed results
    // int width = (int)cvGetCaptureProperty( capture_, CV_CAP_PROP_FRAME_WIDTH );
    //int height = (int)cvGetCaptureProperty( capture_, CV_CAP_PROP_FRAME_HEIGHT );

    if(camera_ == NULL ) 
    {
        stringstream ss;
        ss << "Camera could not be initialized" ;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    
    if( width != static_cast<int>(config_.width) )
    {
        std::stringstream sse;
        sse << "config does not match image properties: actual width=" << width << " config width=" << config_.width;
        throw gbxutilacfr::Exception(ERROR_INFO, sse.str());
    }

    if( height != static_cast<int>(config_.height) )
    {
        std::stringstream sse;
        sse << "config does not match image properties: actual height=" << height << " config height=" << config_.height;
        throw gbxutilacfr::Exception(ERROR_INFO, sse.str());
    }

    if( width*height*3  != static_cast<int>(config_.size) )
    {
        std::stringstream sse;
        sse << "config does not match image properties: actual size=" << width*height*3 << " config size=" << config_.size;
        throw gbxutilacfr::Exception(ERROR_INFO, sse.str());
    }
    
    if( config_.format!="BGR8" && config_.format!="RGB8" && config_.format!="BayerRG8" && config_.format!="BayerGR8" && config_.format!="BayerGB8" && config_.format!="BayerBG8" && config_.format!="GRAY8" )
    {
        std::stringstream sse;
        sse << "config does not match supported image properties: config format=" << config_.format;
        throw gbxutilacfr::Exception(ERROR_INFO, sse.str());

    }

}

Driver::~Driver()
{
    cvReleaseCapture(&camera_);
}

void
Driver::read( hydrointerfaces::Image::Data &data )
{

    context_.tracer().debug( "Grabbing frame(s) from camera(s)..." );
   
    // This performs the grab and the retrieve with one cv function call
    frame_ = cvQueryFrame(camera_);
    if(frame_ == NULL)
    {
        stringstream ss;
        ss << "Failed to retrieve frame from Camera";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    memcpy(data.pixelData, frame_->imageData, frame_->imageSize);
    
    IceUtil::Time t = IceUtil::Time::now();
    data.timeStampSec = (int)t.toSeconds();
    data.timeStampUsec = (int)t.toMicroSeconds() - data.timeStampSec*1000000;
}

void
Driver::setWidthAndHeight(int width, int height)
{
// firewire libraries need a specified format to convert to a particular size. So check width and then
// make sure the height is compatible with the width and then convert to the particular dc1394 format
// that supports the width, height, and format. See <dc1394/types.h> for valid formats.
    
    // find the width and height of the frame
    // workaround for some of opencv grabber functionality...
    // load the grabbed frame into an iplimage struct and then find the width.
    frame_ = cvQueryFrame( camera_);
    if (width==frame_->width && height==frame_->height)
    {
        //do nothing
        return;
    }
    
    int ret = 0;
    switch( width )
    {
        case 160:
            if ( height != 120 )
            {
                std::cout << "ERROR(imagecvcamera/driver.cpp): Images of size 160x" << height << "are not supported. Valid sizes are 160x120" << std::endl; 
            }
            else
            {
                ret = cvSetCaptureProperty( camera_, CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_160x120_YUV444 );
            }
            break;
        case 320:
            if ( height != 240 )
            {
                std::cout << "ERROR(imagecvcamera/driver.cpp): Images of size 320x" << height << "are not supported. Valid sizes are 320x240" << std::endl; 
            }
            {
                ret = cvSetCaptureProperty( camera_, CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_320x240_YUV422 );
            }
            break;
        case 640:
            if ( height != 480 )
            {
                std::cout << "ERROR(imagecvcamera/driver.cpp): Images of size 640x" << height << "are not supported. Valid sizes are 640x480" << std::endl; 
            }
            else
            {
                // set the mode according to the requested format and size
                if ( config_.format=="BayerRG8" || config_.format=="BayerGR8" || config_.format=="BayerGB8" || config_.format=="BayerBG8" || config_.format=="GRAY8" )
                {
                    ret = cvSetCaptureProperty( camera_, CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_640x480_MONO8 );
                } 
                if ( config_.format=="RGB8" || config_.format=="BGR8" )
                {
                    ret = cvSetCaptureProperty( camera_, CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_640x480_RGB8 );
                } 
                // Following formats are not supported yet:
                // DC1394_VIDEO_MODE_640x480_YUV411,
                // DC1394_VIDEO_MODE_640x480_YUV422,
                // DC1394_VIDEO_MODE_640x480_MONO16,
            }
            break;
        case 800:
            if ( height != 600 )
            {
                std::cout << "ERROR(imagecvcamera/driver.cpp): Images of size 800x" << height << "are not supported. Valid sizes are 800x600" << std::endl; 
            }
            else
            {
                if ( config_.format=="BayerRG8" || config_.format=="BayerGR8" || config_.format=="BayerGB8" || config_.format=="BayerBG8" || config_.format=="GRAY8" )
                {
                    ret = cvSetCaptureProperty( camera_, CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_800x600_MONO8 );
                } 
                if ( config_.format=="RGB8" || config_.format=="BGR8" )
                {
                    ret = cvSetCaptureProperty( camera_, CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_800x600_RGB8 );
                } 
                // Following formats are not supported yet:
                // DC1394_VIDEO_MODE_800x600_YUV422,
                // DC1394_VIDEO_MODE_800x600_MONO16,
            }
            break;
        case 1024:
            if ( height != 768 )
            {
                std::cout << "ERROR(imagecvcamera/driver.cpp): Images of size 1024x" << height << "are not supported. Valid sizes are 1024x768" << std::endl; 
            }
            else
            {
                if ( config_.format=="BayerRG8" || config_.format=="BayerGR8" || config_.format=="BayerGB8" || config_.format=="BayerBG8" || config_.format=="GRAY8" )
                {
                    ret = cvSetCaptureProperty( camera_, CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_1024x768_MONO8 );
                } 
                if ( config_.format=="RGB8" || config_.format=="BGR8" )
                {
                    ret = cvSetCaptureProperty( camera_, CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_1024x768_RGB8 );
                } 
                // Following formats are not supported yet:
                // DC1394_VIDEO_MODE_1024x768_YUV422,
                // DC1394_VIDEO_MODE_1024x768_MONO16,
            }
            break;
        default:
            cout<<"ERROR(imagecvcamera/driver.cpp): Don't know how to convert to the requested size: " << config_.width << "x" << config_.height << endl;
            // unsupported formats:
            //     DC1394_VIDEO_MODE_1280x960_YUV422,
            //     DC1394_VIDEO_MODE_1280x960_RGB8,
            //     DC1394_VIDEO_MODE_1280x960_MONO8,
            //     DC1394_VIDEO_MODE_1600x1200_YUV422,
            //     DC1394_VIDEO_MODE_1600x1200_RGB8,
            //     DC1394_VIDEO_MODE_1600x1200_MONO8,
            //     DC1394_VIDEO_MODE_1280x960_MONO16,
            //     DC1394_VIDEO_MODE_1600x1200_MONO16,
            //     DC1394_VIDEO_MODE_EXIF,
            //     DC1394_VIDEO_MODE_FORMAT7_0,
            //     DC1394_VIDEO_MODE_FORMAT7_1,
            //     DC1394_VIDEO_MODE_FORMAT7_2,
            //     DC1394_VIDEO_MODE_FORMAT7_3,
            //     DC1394_VIDEO_MODE_FORMAT7_4,
            //     DC1394_VIDEO_MODE_FORMAT7_5,
            //     DC1394_VIDEO_MODE_FORMAT7_6,
            //     DC1394_VIDEO_MODE_FORMAT7_7
    }
    
    // check that opencv could convert to the correct size
    if ( ret==0 )
    {
        std::cout << "ERROR(imagecvcamera/driver.cpp): The requested image size is not valid." << std::endl;
    }

    

}

} // namespace

extern "C" {
    hydrointerfaces::ImageFactory *createDriverFactory()
    { return new imagecvcamera::Factory; }
}
