/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaice/heartbeater.h>

#include "mainloop.h"

using namespace std;
using namespace orca;

namespace laser2d {

MainLoop::MainLoop( LaserScanner2dI        &laserObj,
                    Driver                 *hwDriver,
                    bool                    compensateRoll,
                    const orcaice::Context &context )
    : laserObj_(laserObj),
      hwDriver_(hwDriver),
      compensateRoll_(compensateRoll),
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
MainLoop::readData( orca::LaserScanner2dDataPtr & data )
{
//     context_.tracer()->debug( "Reading laser data...", 8 );

    //
    // Read from the laser driver
    //            
    if ( hwDriver_->read( data ) ) 
    {
        context_.tracer()->warning( "Problem reading from laser. Re-initializing hardware." );
        
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        hwDriver_->init();
        return;
    }

    // Check that the angleIncrement matches what we expect
//     Driver::Config cfg;
//     hwDriver_->getConfig( cfg );

    // alexm: Config and LaserScanner2dData have this info in diff. forms
//     if ( !NEAR(  cfg.angleIncrement, data->angleIncrement, 1e-5) )
//     {
//         cout << "ERROR(mainloop.cpp): angleIncrement of laser scan returned from driver does not match configured angleIncrement." << endl;
//         cout << "ERROR(mainloop.cpp): config says: " << RAD2DEG(cfg->angleIncrement) << ",laser data says: " << RAD2DEG(data->angleIncrement) << endl; 
//         cout << "ERROR(mainloop.cpp): If you're using the stage driver:" << endl;
//         cout << "                       This happens because of a bug in stage where the simulated laser" << endl;
//         cout << "                       can't be configured on-the-fly.  You have to manually ensure that" << endl;
//         cout << "                       the sicklaser config and the stage laser config match." << endl;
//         cout << "ERROR(mainloop.cpp): If you're using some other driver:" << endl;
//         cout << "                       This shouldn't happen.  Something went wrong." << endl;
//         assert( false ); exit(1);
//     }
//     assert( NEAR( cfg->angleIncrement, data->angleIncrement, 1e-5) );

    // flip the scan left-to-right if we are configured to do so
    if ( compensateRoll_ ) {
        // NOTE: instead of copying around, we should be able to simply change the
        // start bearing and bearing increment.
        std::reverse( data->ranges.begin(), data->ranges.end() );
        std::reverse( data->intensities.begin(), data->intensities.end() );
    }

    laserObj_.localSetData( data );
}

void
MainLoop::run()
{
    LaserScanner2dDataPtr laserData = new LaserScanner2dData;
    orcaice::Heartbeater heartbeater( context_ );

    // Catches all its exceptions.
    activate();

    hwDriver_->init();

    //
    // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
    //            So we have to avoid getting stuck anywhere within this main loop.
    //
    while ( isActive() )
    {
        try 
        {
            readData( laserData );


            if ( heartbeater.isHeartbeatTime() )
            {
                heartbeater.beat( "Laser enabled (?). " + hwDriver_->heartbeatMessage() );
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
    context_.tracer()->debug( "dropping out from run()", 5 );
}

} // namespace
