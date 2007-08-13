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
#include "mainloop.h"

using namespace std;
using namespace orca;

namespace laser2d {

namespace {
    const char *SUBSYSTEM = "MainLoop";
}

MainLoop::MainLoop( orcaifaceimpl::LaserScanner2dIface &laserInterface,
                    const Driver::Config               &config,
                    DriverFactory                      &driverFactory,
                    bool                                compensateRoll,
                    const orcaice::Context             &context )
    : laserInterface_(laserInterface),
      config_(config),
      driver_(0),
      driverFactory_(driverFactory),
      compensateRoll_(compensateRoll),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 20.0 );
    context_.status()->initialising( SUBSYSTEM );
}

MainLoop::~MainLoop()
{
    if ( driver_ ) delete driver_;
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
        context_.status()->heartbeat( SUBSYSTEM );
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
        context_.status()->heartbeat( SUBSYSTEM );
    }
}

void
MainLoop::initialiseDriver()
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 20.0 );

    if ( driver_ ) delete driver_;

    while ( isActive() )
    {
        try {
            context_.tracer()->info( "MainLoop: Initialising driver..." );
            driver_ = driverFactory_.createDriver( config_, context_ );
            return;
        }
        catch ( std::exception &e )
        {
            stringstream ss;
            ss << "MainLoop: Caught exception while initialising driver: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( char *e )
        {
            stringstream ss;
            ss << "MainLoop: Caught exception while initialising driver: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( std::string &e )
        {
            stringstream ss;
            ss << "MainLoop: Caught exception while initialising driver: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( ... )
        {
            stringstream ss;
            ss << "MainLoop: Caught unknown exception while initialising driver";
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));        
    }

    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 1.0 );
}

void
MainLoop::readData( orca::LaserScanner2dDataPtr &laserData )
{
    //
    // Read from the laser driver
    //            
    driver_->read( laserData->ranges, 
                   laserData->intensities,
                   laserData->timeStamp );
    assert( (int)laserData->ranges.size()      == config_.numberOfSamples );
    assert( (int)laserData->intensities.size() == config_.numberOfSamples );

    // flip the scan left-to-right if we are configured to do so
    if ( compensateRoll_ ) {
        // NOTE: instead of copying around, we should be able to simply change the
        // start bearing and bearing increment.
        std::reverse( laserData->ranges.begin(), laserData->ranges.end() );
        std::reverse( laserData->intensities.begin(), laserData->intensities.end() );
    }
}

void
MainLoop::run()
{
    // Set up the laser-scan object
    LaserScanner2dDataPtr laserData = new LaserScanner2dData;
    laserData->minRange     = config_.minRange;
    laserData->maxRange     = config_.maxRange;
    laserData->fieldOfView  = config_.fieldOfView;
    laserData->startAngle   = config_.startAngle;
    laserData->ranges.resize( config_.numberOfSamples );
    laserData->intensities.resize( config_.numberOfSamples );

    // These functions catch their exceptions.
    activate();
    establishInterface();
    initialiseDriver();

    //
    // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
    //            So we have to avoid getting stuck anywhere within this main loop.
    //
    while ( isActive() )
    {
        try 
        {
            // this blocks until new data arrives
            readData( laserData );
            laserInterface_.localSetAndSend( laserData );
            context_.status()->ok( SUBSYSTEM );

            stringstream ss;
            ss << "MainLoop: Read laser data: " << orcaice::toString(laserData);
            context_.tracer()->debug( ss.str(), 5 );

            continue;

        } // end of try
        catch ( Ice::CommunicatorDestroyedException & )
        {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop. Eventually, component will tell us to stop.
        }
        catch ( const Ice::Exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( const std::exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( const std::string &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected string: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );            
        }
        catch ( const char *e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected char *: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }

        // If we got to here there's a problem.
        // Re-initialise the driver.
        initialiseDriver();

    } // end of while

    // Laser hardware will be shut down in the driver's destructor.
    context_.tracer()->debug( "dropping out from run()", 2 );
}

} // namespace
