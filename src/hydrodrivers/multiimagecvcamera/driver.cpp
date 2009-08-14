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

namespace multiimagecvcamera {

Driver::Driver( const Config& cfg, const hydroutil::Context& context ) :
    config_(cfg),
    context_(context)
{
    cameras_.resize( config_.size() );
    frames_.resize( config_.size() );
    
    for( unsigned int i=0; i<cameras_.size(); ++i )
    {
        // picks up the first camera it finds on the firewire bus
        cameras_.at(i) = cvCreateCameraCapture( i );
        if( cameras_.at(i) == NULL ) 
        {
            std::string ss = "Failed to initialize CvCameraDevice " + i ;
            gbxutilacfr::Exception( ERROR_INFO, ss );
            throw ss;
        }
        
        // the default in opencv is CV_CAP_PROP_CONVERT_RGB=1 which causes
        // cameras with bayer encoding to be converted from mono to rgb
        // without using the bayer functions. CV_CAP_PROP_CONVERT_RGB=0
        // keeps the original format.
        cvSetCaptureProperty( cameras_.at(i), CV_CAP_PROP_CONVERT_RGB, 0 );
    
        // assume all the cameras should be set to the same size as camera 0
        setWidthAndHeight(static_cast<int>(config_.at(i).width), static_cast<int>(config_.at(i).height), i );
        
        // record the actual width and height
        frames_.at(i) = cvQueryFrame( cameras_.at(i) );
        // int width = frame_->width;
        // int height = frame_->height;

        // this following call should work in opencv but have had mixed results
        // int width = (int)cvGetCaptureProperty( capture_, CV_CAP_PROP_FRAME_WIDTH );
        //int height = (int)cvGetCaptureProperty( capture_, CV_CAP_PROP_FRAME_HEIGHT );
    
        if(cameras_.at(i) == NULL ) 
        {
            stringstream ss;
            ss << "Camera " << i << " could not be initialized" ;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
        
        if( frames_.at(i)->width != static_cast<int>(config_.at(i).width) )
        {
            std::stringstream sse;
            sse << "config does not match image properties of camera " << i << ": actual width=" << frames_.at(i)->width << " config width=" << config_.at(i).width;
            throw gbxutilacfr::Exception(ERROR_INFO, sse.str());
        }
    
        if( frames_.at(i)->height != static_cast<int>(config_.at(i).height) )
        {
            std::stringstream sse;
            sse << "config does not match image properties " << i << ": actual height=" << frames_.at(i)->height << " config height=" << config_.at(i).height;
            throw gbxutilacfr::Exception(ERROR_INFO, sse.str());
        }
    
        if( frames_.at(i)->width*frames_.at(i)->height*3  != static_cast<int>(config_.at(i).size) )
        {
            std::stringstream sse;
            sse << "config does not match image properties " << i << ": actual size=" << frames_.at(i)->width*frames_.at(i)->height*3 << " config size=" << config_.at(i).size;
            throw gbxutilacfr::Exception(ERROR_INFO, sse.str());
        }
        
        if( config_.at(i).format!="BGR8" 
            && config_.at(i).format!="RGB8" 
            && config_.at(i).format!="BayerRG8" 
            && config_.at(i).format!="BayerGR8" 
            && config_.at(i).format!="BayerGB8" 
            && config_.at(i).format!="BayerBG8" 
            && config_.at(i).format!="GRAY8" )
        {
            std::stringstream sse;
            sse << "config does not match supported image properties " << i << ": config format=" << config_.at(i).format;
            throw gbxutilacfr::Exception(ERROR_INFO, sse.str());
    
        }
    }
}

Driver::~Driver()
{
    for( unsigned int i=0; i<cameras_.size(); ++i )
    {
        cvReleaseCapture( &cameras_.at(i) );
    }
}

void
Driver::read( hydrointerfaces::MultiImage::Data& data )
{

    context_.tracer().debug( "Grabbing frame(s) from camera(s)..." );
   
    for( unsigned int i=0; i<cameras_.size(); ++i )
    {
        // This performs the grab and the retrieve with one cv function call
        frames_.at(i) = cvQueryFrame( cameras_.at(i) );
        if( frames_.at(i) == NULL )
        {
            stringstream ss;
            ss << "Failed to retrieve frame from Camera " << i;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    
        memcpy( data.at(i).pixelData, frames_.at(i)->imageData, frames_.at(i)->imageSize );
        
        IceUtil::Time t = IceUtil::Time::now();
        data.at(i).timeStampSec = (int)t.toSeconds();
        data.at(i).timeStampUsec = (int)t.toMicroSeconds() - data.at(i).timeStampSec*1000000;
    }
}

void
Driver::setWidthAndHeight(int width, int height, int cameraNumber )
{
// firewire libraries need a specified format to convert to a particular size. So check width and then
// make sure the height is compatible with the width and then convert to the particular dc1394 format
// that supports the width, height, and format. See <dc1394/types.h> for valid formats.
    
    // find the width and height of the frame
    // workaround for some of opencv grabber functionality...
    // load the grabbed frame into an iplimage struct and then find the width.
    frames_.at(cameraNumber) = cvQueryFrame( cameras_.at(cameraNumber) );
    if (width==frames_.at(cameraNumber)->width && height==frames_.at(cameraNumber)->height)
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
                ret = cvSetCaptureProperty( cameras_.at(cameraNumber), CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_160x120_YUV444 );
            }
            break;
        case 320:
            if ( height != 240 )
            {
                std::cout << "ERROR(imagecvcamera/driver.cpp): Images of size 320x" << height << "are not supported. Valid sizes are 320x240" << std::endl; 
            }
            {
                ret = cvSetCaptureProperty( cameras_.at(cameraNumber), CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_320x240_YUV422 );
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
                if ( config_.at(cameraNumber).format=="BayerRG8" 
                        || config_.at(cameraNumber).format=="BayerGR8" 
                        || config_.at(cameraNumber).format=="BayerGB8" 
                        || config_.at(cameraNumber).format=="BayerBG8" 
                        || config_.at(cameraNumber).format=="GRAY8" )
                {
                    ret = cvSetCaptureProperty( cameras_.at(cameraNumber), CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_640x480_MONO8 );
                } 
                if ( config_.at(cameraNumber).format=="RGB8" 
                        || config_.at(cameraNumber).format=="BGR8" )
                {
                    ret = cvSetCaptureProperty( cameras_.at(cameraNumber), CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_640x480_RGB8 );
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
                if ( config_.at(cameraNumber).format=="BayerRG8" 
                        || config_.at(cameraNumber).format=="BayerGR8" 
                        || config_.at(cameraNumber).format=="BayerGB8" 
                        || config_.at(cameraNumber).format=="BayerBG8" 
                        || config_.at(cameraNumber).format=="GRAY8" )
                {
                    ret = cvSetCaptureProperty( cameras_.at(cameraNumber), CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_800x600_MONO8 );
                } 
                if ( config_.at(cameraNumber).format=="RGB8" 
                        || config_.at(cameraNumber).format=="BGR8" )
                {
                    ret = cvSetCaptureProperty( cameras_.at(cameraNumber), CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_800x600_RGB8 );
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
                if ( config_.at(cameraNumber).format=="BayerRG8" 
                        || config_.at(cameraNumber).format=="BayerGR8"
                        || config_.at(cameraNumber).format=="BayerGB8" 
                        || config_.at(cameraNumber).format=="BayerBG8" 
                        || config_.at(cameraNumber).format=="GRAY8" )
                {
                    ret = cvSetCaptureProperty( cameras_.at(cameraNumber), CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_1024x768_MONO8 );
                } 
                if ( config_.at(cameraNumber).format=="RGB8" 
                        || config_.at(cameraNumber).format=="BGR8" )
                {
                    ret = cvSetCaptureProperty( cameras_.at(cameraNumber), CV_CAP_PROP_MODE, DC1394_VIDEO_MODE_1024x768_RGB8 );
                } 
                // Following formats are not supported yet:
                // DC1394_VIDEO_MODE_1024x768_YUV422,
                // DC1394_VIDEO_MODE_1024x768_MONO16,
            }
            break;
        default:
            cout<<"ERROR(imagecvcamera/driver.cpp): Don't know how to convert camera " << cameraNumber << "to the requested size: " << config_.at(cameraNumber).width << "x" << config_.at(cameraNumber).height << endl;
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
    hydrointerfaces::MultiImageFactory *createDriverFactory()
    { return new multiimagecvcamera::Factory; }
}
