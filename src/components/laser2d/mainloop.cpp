/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

MainLoop::MainLoop( orcaifaceimpl::LaserScanner2dI &laserInterface,
                    Driver                 *hwDriver,
                    bool                    compensateRoll,
                    const orcaice::Context &context )
    : laserInterface_(laserInterface),
      hwDriver_(hwDriver),
      compensateRoll_(compensateRoll),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( "hardware", 10.0 );
    context_.status()->initialising( "hardware" );
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
            return;
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
        context_.status()->heartbeat( "hardware" );
    }
}

void
MainLoop::establishInterface()
{
    while ( isActive() )
    {
        try {
            laserInterface_.initInterface();
            context_.tracer()->debug( "Activated Laser interface" );
            return;
        }
        catch ( orcaice::Exception &e )
        {
            context_.tracer()->warning( std::string("MainLoop::establishInterface(): ") + e.what() );
        }
        catch ( ... )
        {
            context_.tracer()->warning( "MainLoop::establishInterface(): caught unknown exception." );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        context_.status()->heartbeat( "hardware" );
    }
}

int
MainLoop::readData( orca::LaserScanner2dDataPtr & data )
{
//     context_.tracer()->debug( "Reading laser data...", 8 );

    //
    // Read from the laser driver
    //            
    if ( hwDriver_->read( data ) ) 
    {
        context_.tracer()->warning( "Problem reading from laser. Re-initialising hardware." );
        
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        int ret = hwDriver_->init();
        if ( ret == 0 )
            context_.tracer()->info( "Re-Initialisation succeeded." );
        else
            context_.tracer()->info( "Re-Initialisation failed." );
        return -1;
    }

    // flip the scan left-to-right if we are configured to do so
    if ( compensateRoll_ ) {
        // NOTE: instead of copying around, we should be able to simply change the
        // start bearing and bearing increment.
        std::reverse( data->ranges.begin(), data->ranges.end() );
        std::reverse( data->intensities.begin(), data->intensities.end() );
    }

    return 0;
}

void
MainLoop::run()
{
    LaserScanner2dDataPtr laserData = new LaserScanner2dData;
    orcaice::Heartbeater heartbeater( context_ );

    // Catches all its exceptions.
    activate();
    establishInterface();

    hwDriver_->init();
    context_.status()->setMaxHeartbeatInterval( "hardware", 1.0 );

//    int prevReadResult = 0;
    int currReadResult = 0;

    //
    // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
    //            So we have to avoid getting stuck anywhere within this main loop.
    //
    while ( isActive() )
    {
        try 
        {
            // this blocks until new data arrives
            currReadResult = readData( laserData );

            // make sure we are not shutting down, otherwise we'll segfault while trying to send
            if ( currReadResult == 0 && isActive() ) {
                laserInterface_.localSetAndSend( laserData );
            }

            // Set subsystem status
            if ( currReadResult == 0 )
            {
                context_.status()->ok( "hardware" );
            }
            else
            {
                context_.status()->fault( "hardware", hwDriver_->heartbeatMessage() );
            }

//             // old way to send heartbeats
//             if ( heartbeater.isHeartbeatTime() )
//             {
//                 stringstream ss;
//                 if ( currReadResult == 0 ) {
//                     ss << "Laser enabled. ";
//                 }
//                 else {
//                     ss << "Laser having problems.";
//                 }
//                 heartbeater.beat( ss.str() + hwDriver_->heartbeatMessage() );
//             }

//             // new way to send heartbeats
//             // status has changed
//             if ( currReadResult != prevReadResult ) {
//                 if ( currReadResult==0 ) {
//                     context_.status()->ok( "hardware", "Laser enabled. "+hwDriver_->heartbeatMessage() );
//                 }
//                 else {
//                     context_.status()->fault( "hardware", "Laser having problems. "+hwDriver_->heartbeatMessage() );
//                 }
//             }
//             // status has not changed
//             else {
//                 cout<<"TRACE(mainloop.cpp): setting status." << endl;
//                 orcaice::Status *status = context_.status();
//                 status->heartbeat( "hardware" );
//                 //context_.status()->heartbeat( "hardware" );
//                 cout<<"TRACE(mainloop.cpp): set status." << endl;
//             }
//             prevReadResult = currReadResult;

        } // end of try
        catch ( Ice::CommunicatorDestroyedException & )
        {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop. Eventually, component will tell us to stop.
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
    context_.tracer()->debug( "dropping out from run()", 2 );
}

} // namespace
