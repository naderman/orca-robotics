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
#include <iostream>

#include <orcaice/orcaice.h>
#include <orcaice/heartbeater.h>

using namespace std;
using namespace orca;

namespace laser2d {

namespace {
    const int MAX_TIME_FOR_RECONFIGURE = 20000; // ms
}

MainLoop::MainLoop( LaserScanner2dI        &laserObj,
                    Driver                 *hwDriver,
                    const orcaice::Context &context,
                    bool                    startEnabled,
                    bool                    compensateRoll )
    : laserObj_(laserObj),
      hwDriver_(hwDriver),
      context_(context),
      startEnabled_(startEnabled),
      compensateRoll_(compensateRoll)
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
            cout<<"laser2d::mainloop: activating..." << endl;
            context_.activate();
            cout<<"laser2d::mainloop: activated."<<endl;
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
MainLoop::reconfigure()
{
    RangeScanner2dConfigPtr desiredConfig;

    // get and pop, so we remove the request from the buffer
    laserObj_.desiredConfigBuffer_.getAndPop( desiredConfig );

    context_.tracer()->print( "mainloop: Setting config to: " + orcaice::toString( desiredConfig ) );

    bool configurationDone = false;
    int reconfigCount = 0;
    IceUtil::Time reconfigStartTime = IceUtil::Time::now();
    while ( !configurationDone 
            && isActive()
            && laserObj_.desiredConfigBuffer_.isEmpty() )
    {
        if ( hwDriver_->setConfig( desiredConfig ) == 0 )
        {
            context_.tracer()->print( "Successful reconfiguration! " + hwDriver_->infoMessages() );

            // Tell the world that we've reconfigured
            laserObj_.currentConfigBuffer_.push( desiredConfig );
            configurationDone = true;
        }
        else
        {
            if ( (IceUtil::Time::now()-reconfigStartTime).toMilliSecondsDouble() > MAX_TIME_FOR_RECONFIGURE )
            {
                std::stringstream ss;
                cout << "Couldn't set config: " << orcaice::toString(desiredConfig) << endl;
                ss << "Configuration failed: " << hwDriver_->infoMessages();
                context_.tracer()->warning( ss.str() );
                reconfigStartTime = IceUtil::Time::now();
            }
            else
            {
                context_.tracer()->print( "Still trying to reconfigure..." );
                context_.tracer()->print( hwDriver_->infoMessages() );

                // Try fast a couple of times, then slow down
                if ( reconfigCount>5 ) {
                    IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(300));
                }
                ++reconfigCount;
            }

            // Tell the world that we're down while re-configuring
            RangeScanner2dConfigPtr failedConfig = RangeScanner2dConfigPtr::dynamicCast( desiredConfig->ice_clone() );
            failedConfig->isEnabled = false;
            laserObj_.currentConfigBuffer_.push( failedConfig );
        }
    } // end of configuration loop
}

void
MainLoop::readLaserData( orca::LaserScanner2dDataPtr &laserData )
{
    context_.tracer()->debug( "Reading laser data...", 8 );

    //
    // Read from the laser driver
    //
    int ret = hwDriver_->read( laserData );
            
    if ( ret != 0 )
    {
        context_.tracer()->error( "Problem reading from laser.  Shutting down hardware." );
        hwDriver_->disable();

        RangeScanner2dConfigPtr cfg;
        laserObj_.currentConfigBuffer_.get( cfg );

        // Tell the laser to try to get back to this config
        laserObj_.desiredConfigBuffer_.push( cfg );

        // If anyone out there asks, our config has changed.
        cfg->isEnabled = false;
        laserObj_.currentConfigBuffer_.push( cfg );
    }
    else
    {
        // Check that the angleIncrement matches what we expect
//alexm todo:
//         RangeScanner2dConfigPtr cfg;
//         laserObj_.currentConfigBuffer_.get( cfg );
//         if ( !NEAR(  cfg->angleIncrement, laserData->angleIncrement, 1e-5) )
//         {
//             cout << "ERROR(mainloop.cpp): angleIncrement of laser scan returned from driver does not match configured angleIncrement." << endl;
//             cout << "ERROR(mainloop.cpp): config says: " << RAD2DEG(cfg->angleIncrement) << ",laser data says: " << RAD2DEG(laserData->angleIncrement) << endl; 
//             cout << "ERROR(mainloop.cpp): If you're using the stage driver:" << endl;
//             cout << "                       This happens because of a bug in stage where the simulated laser" << endl;
//             cout << "                       can't be configured on-the-fly.  You have to manually ensure that" << endl;
//             cout << "                       the sicklaser config and the stage laser config match." << endl;
//             cout << "ERROR(mainloop.cpp): If you're using some other driver:" << endl;
//             cout << "                       This shouldn't happen.  Something went wrong." << endl;
//             assert( false ); exit(1);
//         }
//         assert( NEAR( cfg->angleIncrement, laserData->angleIncrement, 1e-5) );

        // flip the scan left-to-right if we are configured to do so
        if ( compensateRoll_ ) {
            // NOTE: instead of copying around, we should be able to simply change the
            // start bearing and bearing increment.
            std::reverse( laserData->ranges.begin(), laserData->ranges.end() );
            std::reverse( laserData->intensities.begin(), laserData->intensities.end() );
        }

        laserObj_.localSetData( laserData );
    }
}

void
MainLoop::run()
{
    LaserScanner2dDataPtr laserData = new LaserScanner2dData;
    orcaice::Heartbeater heartbeater( context_ );

    // Catches all its exceptions.
    activate();

    //
    // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
    //            So we have to avoid getting stuck in a loop anywhere within this main loop.
    //
    while ( isActive() )
    {
        try 
        {
            // Service config requests
            while ( !laserObj_.desiredConfigBuffer_.isEmpty() && isActive() )
            {
                reconfigure();
            }

            //
            // This 'if' block is what slows the loop down, by either reading from the laser
            // or sleeping.
            //
            if ( hwDriver_->isEnabled() )
            {
                readLaserData( laserData );
            }
            else
            {
                // Wait for someone to enable us
                IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));
            }

            if ( heartbeater.isHeartbeatTime() )
            {
                if ( hwDriver_->isEnabled() )
                {
                    heartbeater.beat( "Laser enabled. " + hwDriver_->heartbeatMessage() );
                }
                else
                {
                    heartbeater.beat( "Laser disabled." );
                }
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
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
            context_.tracer()->error( ss.str() );
        }

    } // end of while

    // Laser hardware will be shut down in the driver's destructor.
    context_.tracer()->info( "dropping out from run()", 5 );
}

}
