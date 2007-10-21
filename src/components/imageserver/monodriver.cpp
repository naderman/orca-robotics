/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <stdlib.h>
#include <orca/camera.h>
#include <orcaice/orcaice.h>

//#include <triclops/pnmutils.h>

// image mode conversions
#include <orcaimage/conversions.h>
#include <orcaimage/imageutils.h>

#include "monodriver.h"

using namespace std;

namespace imageserver {

MonoDriver::MonoDriver( orcaimage::ImageGrabber* imageGrabber, 
                        const Config& cfg, 
                        const orcaice::Context& context ) : 
    Driver(cfg, context),
    imageGrabber_(imageGrabber),
    context_(context)   
{
}

MonoDriver::~MonoDriver()
{
}

int
MonoDriver::init()
{
    // set width and height
    if ( config_.imageWidth == 0 || config_.imageHeight == 0 )
    {
        // use default if user has not specified anything
        config_.imageWidth = imageGrabber_->width();
        config_.imageHeight = imageGrabber_->height();
    }
    else
    {
        // user specified width and height
        imageGrabber_->setWidth( config_.imageWidth );
        imageGrabber_->setHeight( config_.imageHeight );
        context_.tracer()->debug("TODO(monodriver::init()): there should be a check here that the image size is compatible with the hardware", 3);
        
        // workaround for setting width and height for firewire cameras
        // opencv requires the mode to be set for the correct width and height to be set also
        if ( (imageGrabber_->width() != config_.imageWidth) || (imageGrabber_->height() !=  config_.imageHeight) )
        {
            // check the mode
            orca::ImageFormat mode = orcaimage::orcaImageMode( imageGrabber_->mode() );
            // find the dc1394 specific mode
            int dc1394Mode = orcaimage::dc1394ImageMode( mode, config_.imageWidth, config_.imageHeight );
            if ( dc1394Mode > 0)
                imageGrabber_->setMode( dc1394Mode );
            else
            {
                std::string errString = "Unknown colour mode";
                context_.tracer()->error( errString );
                // this will kill this component
                throw hydroutil::Exception( ERROR_INFO, errString );
            }
        }
    }
    
    // set the format
    if( config_.format == orca::ImageFormatBayerBg | config_.format == orca::ImageFormatBayerGb | config_.format == orca::ImageFormatBayerRg | config_.format == orca::ImageFormatBayerGr )
    {
    }
    else if ( config_.format == orca::ImageFormatDigiclopsStereo | config_.format == orca::ImageFormatDigiclopsRight | config_.format == orca::ImageFormatDigiclopsBoth )
    {
        // tell the digiclops what type of images to send
        imageGrabber_->setMode( config_.format );
    }
    else
    {
        // let the grabber figure out the format if no bayer encoding or not using a digiclops camera
        config_.format = orcaimage::orcaImageMode( imageGrabber_->mode() );
    }
    
    // Setup the rest of camera config
    config_.frameRate = imageGrabber_->fps();

    // this is never read as opencv deals with it
    config_.compression = orca::ImageCompressionNone;

    // query the image grabber for the actual image size
    config_.imageSize = imageGrabber_->size();
    if( config_.imageSize <= 0 )
    {
        std::string errString = "Image size is <= 0. It must be a positive value.";
        context_.tracer()->error( errString );
        // this will kill this component
        throw hydroutil::Exception( ERROR_INFO, errString );
    }

    context_.tracer()->info( "MonoOpenCVDriver has now been configured to: "+config_.toString() );
   
    return 0;
}

int 
MonoDriver::read( orca::CameraData& data )
{
    context_.tracer()->debug( "Grabbing camera data...",8 );
    
    //grab the image and load it into the orca object
    char* rawImage = imageGrabber_->queryFrame();
    if ( rawImage == NULL )
    {
        std::string errString = "Unable to get image from image grabber";
        context_.tracer()->error( errString );
        // this will kill this component
        throw hydroutil::Exception( ERROR_INFO, errString );
        // exit(1);
    }
    else
    {
        //triclops test stuff
        
//         TriclopsInput triclopsInput;
//         triclopsInput.nrows = 768;
//         triclopsInput.ncols = 1024;
//         triclopsInput.inputType = TriInp_RGB;
//         int imageSize = triclopsInput.nrows * triclopsInput.ncols;
//         vector<char> red;
//         vector<char> green;
//         vector<char> blue;
//         red.resize(imageSize);
//         green.resize(imageSize);
//         blue.resize(imageSize);

//         // the R,G, and B values are stored in separate arrays
//         memcpy(&red[0], rawImage, imageSize );
//         memcpy(&green[0], rawImage+imageSize, imageSize );
//         memcpy(&blue[0], rawImage+imageSize*2, imageSize );

//         triclopsInput.u.rgb.red = &red[0];
//         triclopsInput.u.rgb.green = &green[0];
//         triclopsInput.u.rgb.blue = &blue[0];

//         char file[] = "test2.ppm";
//         bool res = ppmWriteFromTriclopsInput( file , &triclopsInput );
//         cout << "res: " << res << endl;

        // set the time stamp as soon as we get the image from the imagegrabber.
        // this is inexact... is there a better way?
        orcaice::setToNow( data.timeStamp );


        // size of the image (data->image.size()) was defined in mainloop.cpp
        memcpy( &data.image[0], rawImage, data.image.size() );

        return 0;
    }
}

}
