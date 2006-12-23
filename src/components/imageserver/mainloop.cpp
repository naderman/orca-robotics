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
MainLoop::readData( orca::CameraData& data )
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
    try
    {   
    
        CameraData cameraData;
        // Copy config parameters into static object fields
        // Only need to do this once
        hwDriver_->initData( cameraData );
        
        orcaice::Heartbeater heartbeater( context_ );

        // Catches all its exceptions.
        activate();

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
                ss << "ERROR(mainloop::run()): Caught unexpected Ice exception: " << e;
                context_.tracer()->error( ss.str() );
            }
            catch ( std::exception &e )
            {
                std::stringstream ss;
                ss << "ERROR(mainloop::run()): Caught unexpected std::exception: ";
                context_.tracer()->error( ss.str() );
            }
            catch ( ... )
            {
                std::stringstream ss;
                ss << "ERROR(mainloop::run()): Caught unexpected unknown exception.";
                context_.tracer()->error( ss.str() );
            }

        } // end of while

        // Camera hardware will be shut down in the driver's destructor.
        context_.tracer()->debug( "dropping out from run()", 5 );

    } // end of try
    catch ( Ice::Exception &e )
    {
        std::stringstream ss;
        ss << "ERROR(mainloop::run()): Caught unexpected Ice exception: " << e;
        context_.tracer()->error( ss.str() );
    }
    catch ( std::exception &e )
    {
        std::stringstream ss;
        ss << "ERROR(mainloop::run()): Caught unexpected std::exception: ";
        context_.tracer()->error( ss.str() );
    }
    catch ( ... )
    {
        std::stringstream ss;
        ss << "ERROR(mainloop::run()): Caught unexpected unknown exception.";
        context_.tracer()->error( ss.str() );
    }
    
}

} // namespace
