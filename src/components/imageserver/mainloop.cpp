/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaice/heartbeater.h>

#include "mainloop.h"
// #include "conversions.h"

using namespace orca;

namespace imageserver {

MainLoop::MainLoop( CameraI&                cameraObj,
                    Driver*                 hwDriver,
                    ImageGrabber*           imageGrabber,
                    const orcaice::Context& context )
    : cameraObj_(cameraObj),
      hwDriver_(hwDriver),
      imageGrabber_(imageGrabber),
      context_(context)
{
}

MainLoop::~MainLoop()
{
}

void
MainLoop::activate()
{
    while ( isActive() )
    {
        try {
            context_.activate();
            break;
        }
        catch ( orcaice::Exception & e )
        {
            std::stringstream ss;
            ss << "MainLoop::activate(): Caught exception: " << e.what();
            context_.tracer()->warning( ss.str() );
        }
        catch ( Ice::Exception & e )
        {
            std::stringstream ss;
            ss << "MainLoop::activate(): Caught exception: " << e;
            context_.tracer()->warning( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->warning( "MainLoop::activate(): caught unknown exception." );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void
MainLoop::readData( orca::CameraDataPtr & data )
{
     context_.tracer()->debug( "Reading data...", 8 );

    //
    // Read from the driver
    //            
    if ( hwDriver_->read( data ) ) 
    {
        context_.tracer()->warning( "Problem reading from the sensor. Re-initializing hardware." );
        
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        hwDriver_->init();

        // copy config parameters into object fields
        hwDriver_->initData( data );

        return;
    }

    cameraObj_.localSetData( data );
}

void
MainLoop::run()
{
    CameraDataPtr cameraData = new CameraData;
    // Copy config parameters into static object fields
    // Only need to do this once
    hwDriver_->initData( cameraData );
    
    orcaice::Heartbeater heartbeater( context_ );

    // Catches all its exceptions.
    activate();

    // ben: remove if it works from component.
//     hwDriver_->init();

    //
    // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
    //            So we have to avoid getting stuck anywhere within this main loop.
    //
    while ( isActive() )
    {
        try 
        {
            readData( cameraData );


            if ( heartbeater.isHeartbeatTime() )
            {
                heartbeater.beat( hwDriver_->heartbeatMessage() );
            }

        } // end of try
        catch ( Ice::CommunicatorDestroyedException & )
        {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop.
        }
        catch ( Ice::Exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected Ice exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected std::exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
            context_.tracer()->error( ss.str() );
        }

    } // end of while

    // Camera hardware will be shut down in the driver's destructor.
    context_.tracer()->debug( "dropping out from run()", 5 );
}

void
MainLoop::initialiseCamera()
{                            
//     // set width and height
//     if ( desiredConfig->imageWidth == 0 || desiredConfig->imageHeight == 0 )
//     {
//         // use default if user has not specified anything
//         cameraData->imageWidth = imageGrabber_->width();
//         cameraData->imageHeight = imageGrabber_->height();
//     }
//     else
//     {
//         std::cout << "TODO(mainloop.cpp): there should be a check here that the image size is compatible with the hardware" << std::endl;
//         // user specified width and height
//         cameraData->imageWidth = desiredConfig->imageWidth;
//         cameraData->imageHeight = desiredConfig->imageHeight;
//         imageGrabber_->setWidth( desiredConfig->imageWidth );
//         imageGrabber_->setHeight( desiredConfig->imageHeight );
//         
//         // workaround for setting width and height for firewire cameras
//         // opencv requires the mode to be set for the correct width and height to be set also
//        if ( (imageGrabber_->width() != desiredConfig->imageWidth) || (imageGrabber_->height() !=  desiredConfig->imageHeight) )
//        {
//             // check the mode
//             orca::ImageFormat mode = orcaImageMode( imageGrabber_->mode() );
//             // find the dc1394 specific mode
//             int dc1394Mode = imageserver::dc1394ImageMode( mode, desiredConfig->imageWidth, desiredConfig->imageHeight );
//             if ( dc1394Mode > 0)
//                 imageGrabber_->setMode( dc1394Mode );
//             else
//             {
//                 // TODO: throw an exception instead of exiting
//                 std::cout << "ERROR(mainloop.cpp): unknown colour mode" << std::endl;
//                 exit(1);
//             }
//         }
//     }
//     
//     // set the format
//     if( desiredConfig->format == ImageFormatBayerBg | desiredConfig->format == ImageFormatBayerGb | desiredConfig->format == ImageFormatBayerRg | desiredConfig->format == ImageFormatBayerGr )
//     {
//         // force the format to be bayer
//         cameraData->format = desiredConfig->format;
// 
//     }
//     else if ( desiredConfig->format == ImageFormatDigiclopsStereo | desiredConfig->format == ImageFormatDigiclopsRight | desiredConfig->format == ImageFormatDigiclopsBoth )
//     {
//         // set digiclops format
//         cameraData->format = desiredConfig->format;
//     
//         // tell the digiclops what type of images to send
//         imageGrabber_->setMode( desiredConfig->format );
//     }
//     else
//     {
//         // let the grabber figure out the format if no bayer encoding or not using a digiclops camera
//         cameraData->format = orcaImageMode( imageGrabber_->mode() );
//         // include this in camera config
//         desiredConfig->format = cameraData->format;
//     }
//     std::cout << "imageGrabber_->width(): " << imageGrabber_->width() << std::endl;
//     std::cout << "imageGrabber_->height(): " << imageGrabber_->height() << std::endl;
//     std::cout << "cameraData->format: " <<  cameraData->format << std::endl;    
// 
//     // resize the object for the correct image size
//     cameraData->image.resize( imageGrabber_->size() );
//     
//     // Setup the rest of camera config
//     desiredConfig->imageWidth = cameraData->imageWidth;
//     desiredConfig->imageHeight = cameraData->imageHeight;
//     desiredConfig->frameRate = imageGrabber_->fps();
}

} // namespace
