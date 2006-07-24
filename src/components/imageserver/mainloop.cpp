/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "mainloop.h"
#include "conversions.h"

#include <orcaice/orcaice.h>

//using namespace std;
using namespace orca;

namespace imageserver {

MainLoop::MainLoop( CameraI            &cameraObj,
                    Driver*            hwDriver,
                    orcaice::Context   context,
                    ImageGrabber*      imageGrabber,
                    bool               startEnabled, 
                    std::string        driverName )
    : cameraObj_(cameraObj),
      hwDriver_(hwDriver),
      context_(context),
      imageGrabber_(imageGrabber),
      driverName_(driverName)
{
}

MainLoop::~MainLoop()
{
}

void
MainLoop::run()
{
    const int MAX_TIME_FOR_RECONFIGURE = 20000; // ms
    const int TIME_BETWEEN_HEARTBEATS  = 10000;  // ms
    IceUtil::Time lastHeartbeatTime = IceUtil::Time::now();

    try 
    {
        CameraDataPtr cameraData = new CameraData;

        //
        // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
        //            So we have to avoid getting stuck in a loop anywhere within this main loop.
        //
        while ( isActive() )
        {
            //
            // This big while loop checks for config requests
            //
            while ( !cameraObj_.desiredConfigBuffer_.isEmpty() && isActive() )
            {
                CameraConfigPtr desiredConfig;

                // get and pop, so we remove the request from the buffer
                cameraObj_.desiredConfigBuffer_.getAndPop( desiredConfig );

//                context_.tracer()->print( "Setting config to: " + orcaice::toString( desiredConfig ) );

                bool configurationDone = false;
                IceUtil::Time reconfigStartTime = IceUtil::Time::now();
                while ( !configurationDone 
                        && isActive()
                        && cameraObj_.desiredConfigBuffer_.isEmpty() )
                {
                    if ( hwDriver_->setConfig( desiredConfig ) == 0 )
                    {
                        context_.tracer()->print( "Successful reconfiguration! " + hwDriver_->infoMessages() );

                        // Automatically setup Camera object if we're
                        // not using the fake driver. Fake driver
                        // manually sets these without the use of
                        // imagegrabber.
                        //  Only need to do this once.
                        if( driverName_ != "fake")
                        {
                            cameraData->imageWidth = imageGrabber_->getWidth();
                            cameraData->imageHeight = imageGrabber_->getHeight();
                            cameraData->image.resize( imageGrabber_->getSize() );
                            if( desiredConfig->format == BAYERBG | desiredConfig->format == BAYERGB | desiredConfig->format == BAYERRG | desiredConfig->format == BAYERGR ) 
                            {
                                // force the format to be bayer
                                cameraData->format = desiredConfig->format;
                            }
                            else if ( desiredConfig->format == DIGICLOPSSTEREO | desiredConfig->format == DIGICLOPSRIGHT | desiredConfig->format == DIGICLOPSBOTH )
                            {
                                // set the format
                                cameraData->format = desiredConfig->format;

                                // tell the digiclops what type of images to send
                                imageGrabber_->setMode( desiredConfig->format );
                            }
                            else
                            {
                                // let the grabber figure out the format if no bayer encoding or not using a digiclops camera
                                cameraData->format = orcaImageMode( imageGrabber_->getMode() );
                                // include this in camera config 
                                desiredConfig->format = cameraData->format;
                            }

                            // Setup the rest of camera config 
                            desiredConfig->imageWidth = cameraData->imageWidth;
                            desiredConfig->imageHeight = cameraData->imageHeight;
                            desiredConfig->frameRate = imageGrabber_->getFps();
                        }
                       
                        context_.tracer()->print( "Automatic config has the following settings: " + orcaice::toString( desiredConfig ) );

                        // Tell the world that we've reconfigured
                        cameraObj_.currentConfigBuffer_.push( desiredConfig );
                        configurationDone = true;
                    }
                    else
                    {
                        if ( (IceUtil::Time::now()-reconfigStartTime).toMilliSecondsDouble() > MAX_TIME_FOR_RECONFIGURE )
                        {
                            context_.tracer()->error( "Configuration failed: " + orcaice::toString(desiredConfig) );
                            context_.tracer()->error( hwDriver_->infoMessages() );
                        }
                        else
                        {
                            context_.tracer()->print( "Still trying to reconfigure..." );
                            context_.tracer()->print( hwDriver_->infoMessages() );
                        }

                        // Tell the world that we're down while re-configuring
                        CameraConfigPtr failedConfig = CameraConfigPtr::dynamicCast( desiredConfig->ice_clone() );
                        failedConfig->isEnabled = false;
                        cameraObj_.currentConfigBuffer_.push( failedConfig );
                    }
                } // end of configuration loop
            } // end of check for config requires


            //
            // This 'if' block is what slows the loop down, by either reading from the cmaera
            // or sleeping.
            //
            if ( hwDriver_->isEnabled() )
            {
                // Read from the camera
                int ret = hwDriver_->read( cameraData );
                if ( ret != 0 )
                {
                    context_.tracer()->error( "Problem reading from camera.  Shutting down hardware." );
                    hwDriver_->disable();

                    CameraConfigPtr cfg;
                    cameraObj_.currentConfigBuffer_.get( cfg );

                    // Tell the camera to try to get back to this config
                    cameraObj_.desiredConfigBuffer_.push( cfg );

                    // Tell the world what our current configuration is
                    cfg->isEnabled = false;
                    cameraObj_.currentConfigBuffer_.push( cfg );
                }
                else
                {
                    cameraObj_.localSetData( cameraData );
                }
            }
            else
            {
                // Wait for someone to enable us
                IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));
            }

            if ( (IceUtil::Time::now()-lastHeartbeatTime).toMilliSecondsDouble() >= TIME_BETWEEN_HEARTBEATS )
            {
                if ( hwDriver_->isEnabled() )
                {
                    context_.tracer()->heartbeat("Camera enabled. " + hwDriver_->heartbeatMessage() );
                }
                else
                {
                    context_.tracer()->heartbeat( "Camera disabled." );
                }
                lastHeartbeatTime = IceUtil::Time::now();
            }
        } // end of while
    } // end of try
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
        //
        // Could probably handle it better for an Application by stopping the component on Ctrl-C
        // before shutting down communicator.
    }

    // Camera hardware will be shut down in the driver's destructor.
    context_.tracer()->debug( "dropping out from run()", 5 );
}



}
